#include "Gui/mainwindow.h"

#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <vector>
#include <queue>
#include <QThread>
#include <mutex>
#include <pulse/pulseaudio.h>
#include "Network/NetSocket.h"
#include "Settings/Settings.h"
#include "Audio/PulseAudioHandler.h"

#define RATE 48000
#define addr addr_work

const QString addr_work = "192.9.206.60";
const QString addr_home = "192.168.0.102";

pa_threaded_mainloop *mloop = nullptr;
pa_channel_map* map = nullptr;

std::mutex mutexMainBuff;
uint32_t lenMainBuff = 0;
std::queue<uint8_t> queueBuff;

uint8_t* mainBuff = nullptr;
NetSocket* sock = nullptr;


void stream_state_cb(pa_stream *s, void *mainloop);

void on_state_change1(pa_context *context, void *userdata)
{
    switch(pa_context_get_state(context))
    {
    case PA_CONTEXT_READY:
        qDebug() << "PULSE AUDIO CONNECT. PA_CONTEXT_READY";
        pa_threaded_mainloop_signal(mloop, 0);
        break;

    case PA_CONTEXT_FAILED:
        qDebug() << "PA_CONTEXT_FAILED";
        break;

    case PA_CONTEXT_TERMINATED:
        qDebug() << "PULSE AUDIO DISCONNECT. PA_CONTEXT_TERMINATED";
        pa_threaded_mainloop_signal(mloop, 0);
        break;
    }
}

void on_dev_sink(pa_context *c, const pa_sink_info *info, int eol, void *udata)
{
    if (eol != 0) {
        pa_threaded_mainloop_signal(mloop, 0);
        return;
    }

    const char *device_id = info->name;
    qDebug() << "on_dev_sink:   " << device_id;
}

void on_dev_source(pa_context *c, const pa_source_info *info, int eol, void *udata)
{
    if (eol != 0) {
        pa_threaded_mainloop_signal(mloop, 0);
        return;
    }

    const char *device_id = info->name;
    qDebug() << "on_dev_source:   " << device_id;
}

void on_o_complete(pa_stream *stream, size_t requested_bytes, void *udata)
{
//    if (requested_bytes > 1500)
//        pa_stream_flush(stream,nullptr,nullptr);

    static int as = 0;
    as ++;
    qDebug() << as;
    if (as == 800)
    {
        map->map[0] = PA_CHANNEL_POSITION_FRONT_RIGHT;   //PA_CHANNEL_POSITION_FRONT_LEFT
        map->map[1] = PA_CHANNEL_POSITION_FRONT_RIGHT;    //PA_CHANNEL_POSITION_FRONT_RIGHT
    }
    else if ( as == 1500)
    {
        map->map[0] = PA_CHANNEL_POSITION_FRONT_LEFT;   //PA_CHANNEL_POSITION_FRONT_LEFT
        map->map[1] = PA_CHANNEL_POSITION_FRONT_LEFT;    //PA_CHANNEL_POSITION_FRONT_RIG
    }

    std::unique_lock<std::mutex> lock(mutexMainBuff);

    if (queueBuff.empty())
    {
        pa_stream_flush(stream,nullptr,nullptr);
        return;
    }

    size_t bytesToFill = queueBuff.size();
    if (bytesToFill > requested_bytes)  bytesToFill = requested_bytes;
    //qDebug() << "i want write " << bytesToFill << "bytes, when i can write only " << requested_bytes <<" bytes";
    uint8_t* buffer;
    pa_stream_begin_write(stream, (void**) &buffer, &bytesToFill);


    for (int i = 0; i < bytesToFill; ++i)
    {
        buffer[i] = queueBuff.front();
        queueBuff.pop();
    }

    lock.unlock();


    pa_stream_write(stream, buffer, bytesToFill, nullptr, 0, PA_SEEK_RELATIVE);


//----------------- Неработающий вариант -------------------------
//    std::unique_lock<std::mutex> lock(mutexMainBuff);

//    if (lenMainBuff == 0)
//        return;
//    qDebug() << "Writed " << lenMainBuff << "bytes, when i can write only " << requested_bytes <<" bytes";
//    size_t bytesToFill = lenMainBuff;
//    uint8_t* buffer;

//    if (bytesToFill > requested_bytes)  bytesToFill = requested_bytes;

//    pa_stream_begin_write(stream, (void**) &buffer, &bytesToFill);

//    memcpy(buffer, mainBuff, bytesToFill);

//    lenMainBuff = 0;

//    lock.unlock();

//    pa_stream_write(stream, buffer, bytesToFill, nullptr, 0, PA_SEEK_RELATIVE);


//------ Работающий вариант с приемом прямо внутри --------------------
//    size_t bytes_remaining = requested_bytes;

//    while (bytes_remaining > 0)
//    {
//        char *buffer = nullptr;
//        size_t bytes_to_fill = 1024;
//        if (bytes_to_fill > bytes_remaining) bytes_to_fill = bytes_remaining;
//        pa_stream_begin_write(stream, (void**) &buffer, &bytes_to_fill);
//        if (!bytes_to_fill) return;
//        int64_t res = sock->read(buffer, bytes_to_fill);
//        if (res == -1)
//        {
//            qDebug() << "return";
//            pa_stream_cancel_write(stream);
//            return;
//        }
//        qDebug() << "YES DATA";
//        pa_stream_write(stream, buffer, res, nullptr, 0, PA_SEEK_RELATIVE);
//        bytes_remaining -= bytes_to_fill;
//    }
}

void on_i_complete(pa_stream *stream, size_t nbytes, void *udata)
{
    while (true)
    {
        const void* data;
        size_t n;
        pa_stream_peek(stream, &data, &n);
        if (data == NULL && n == 0)
            return;  // Buffer is empty. Process more events
        else if (data == NULL && n != 0)
        {
            // Buffer overrun occurred
            qDebug() << "Buffer overrun occurred";
            break;
        }
        else
        {
            uint8_t* ptr = (uint8_t*)data;
            for (int i = 0; i < n;)
            {
                int bytes = ((n - i) < 1024) ? n - i : 1024;
                sock->send((void*)ptr,bytes, addr, 1234);
                //qDebug() << "Sended " << bytes << " bytes";
                i += bytes;
                ptr = ptr + bytes;
            }
        }
        pa_stream_drop(stream);
    }
}


//void on_op_complete(pa_stream *s, int success, void *udata)
//{
//    pa_threaded_mainloop_signal(mloop2, 0);
//    qDebug() << "on_op_complete";
//}

void stream_state_cb(pa_stream *s, void *mainloop)
{
    pa_threaded_mainloop_signal((pa_threaded_mainloop*)mainloop, 0);
}



int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    QApplication::setOrganizationName( "Vniira" );
    QApplication::setApplicationName( "RadioSim" );

    pulse::PulseAudioHandler::instance();

    sock = new NetSocket(addr,1234);
    mloop = pa_threaded_mainloop_new();

    pa_threaded_mainloop_lock(mloop);
    pa_threaded_mainloop_start(mloop);
    pa_mainloop_api *apiRead = pa_threaded_mainloop_get_api(mloop);
    pa_context *ctx = pa_context_new_with_proplist(apiRead, "123", nullptr);
    void *udataRead = nullptr;
    pa_context_set_state_callback(ctx, on_state_change1, nullptr);
    pa_context_connect(ctx, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
    pa_threaded_mainloop_wait(mloop); //wait connect


//-----------Устройства------------------
    pa_operation *operationSink;
    pa_operation *operationSource;
    void* udataSink = nullptr;
    void* udataSource = nullptr;

    operationSink = pa_context_get_sink_info_list(ctx, on_dev_sink, nullptr);
    while (true)
    {
        int result = pa_operation_get_state(operationSink);
        if (result == PA_OPERATION_DONE || result == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(mloop);
    }
    pa_operation_unref(operationSink);

    operationSource = pa_context_get_source_info_list(ctx, on_dev_source, nullptr);
    while (true)
    {
        int result = pa_operation_get_state(operationSource);
        if (result == PA_OPERATION_DONE || result == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(mloop);
    }
    pa_operation_unref(operationSource);
//-----------Устройства------------------



    // [1] Настройка каналов
    pa_sample_spec spec;
    spec.format = PA_SAMPLE_U8;
    spec.rate = RATE;
    spec.channels = 2;
    //pa_channel_map map;     //Тут смотреть левый правый похоже
    map = new pa_channel_map;
    pa_channel_map_init_stereo(map);
    map->map[0] = PA_CHANNEL_POSITION_FRONT_LEFT;   //PA_CHANNEL_POSITION_FRONT_LEFT
    map->map[1] = PA_CHANNEL_POSITION_FRONT_LEFT;    //PA_CHANNEL_POSITION_FRONT_RIGHT

    pa_buffer_attr attr;
    attr.maxlength = (uint32_t) -1;
    attr.tlength = 1024;
    attr.prebuf = 0;
    attr.minreq = 1024;
    attr.fragsize = 1024;


    pa_buffer_attr attrRead;
    attrRead.maxlength = (uint32_t) -1;
    attrRead.tlength = (uint32_t) -1;
    attrRead.prebuf = (uint32_t) -1;
    attrRead.minreq = (uint32_t) -1;
    attrRead.fragsize = 1024;

    pa_buffer_attr attrOut;
    attrOut.maxlength = (uint32_t) -1;
    attrOut.tlength = 1024;
    attrOut.prebuf = 0;
    attrOut.minreq = 1024;
    attrOut.fragsize = (uint32_t) -1;

    pa_stream_flags_t stream_flags = pa_stream_flags_t(PA_STREAM_START_CORKED | PA_STREAM_INTERPOLATE_TIMING |
    PA_STREAM_NOT_MONOTONIC | PA_STREAM_AUTO_TIMING_UPDATE | PA_STREAM_ADJUST_LATENCY);

    const char *device_id = nullptr;
    // [1]

    pa_stream *stream = pa_stream_new(ctx, "MyAudioProjectRead", &spec, map);
    void* dataRead = nullptr;
    pa_stream_set_state_callback(stream, stream_state_cb, mloop);
    pa_stream_set_read_callback(stream, on_i_complete, dataRead);
    if (pa_stream_connect_record(stream, device_id, &attr, pa_stream_flags_t(PA_STREAM_ADJUST_LATENCY | PA_STREAM_START_MUTED)) != 0)
        return -5; //not success
    while (true)
    {
        int r = pa_stream_get_state(stream);
        if (r == PA_STREAM_READY)
            break;
        else if (r == PA_STREAM_FAILED)
        {
            qDebug() << "PA_STREAM_FAILED";
            return 0;
        }
        pa_threaded_mainloop_wait(mloop);
    }

    pa_stream *streamOut = pa_stream_new(ctx, "MyAudioProjectOut", &spec, map);
    void* dataOut = nullptr;
    pa_stream_set_write_callback(streamOut, on_o_complete, mloop);
    pa_stream_set_state_callback(streamOut, stream_state_cb, mloop);
    pa_stream_connect_playback(streamOut, device_id, &attr, stream_flags, nullptr, nullptr);
    while (true)
    {
        int r = pa_stream_get_state(streamOut);
        if (r == PA_STREAM_READY)
            break;
        else if (r == PA_STREAM_FAILED)
        {
            qDebug() << "PA_STREAM_FAILED";
            return 0;
        }
        pa_threaded_mainloop_wait(mloop);
    }

    pa_threaded_mainloop_unlock(mloop);
    pa_stream_cork(streamOut, 0, 0, mloop);

    qDebug() << "START WORKING";

    app.exec();
    while(true)
    {
        pa_stream_cork(stream, 1, 0, mloop);

        QThread::sleep(3);

        pa_stream_cork(stream, 0, 0, mloop);

        QThread::sleep(3);

    }
    qDebug() << "STOP WORKING";
   // pa_stream_cork(streamOut, 1, 0, mloop);
    pa_threaded_mainloop_lock(mloop); //
    pa_stream_disconnect(stream);
    pa_stream_unref(stream);

//    pa_stream *streamOut = pa_stream_new(ctx, "MyAudioProjectOut", &spec, &map);
//    void* dataOut = nullptr;
//    pa_stream_set_write_callback(streamOut, on_o_complete, mloop);
//    pa_stream_set_state_callback(streamOut, stream_state_cb, mloop);
//    pa_stream_connect_playback(streamOut, device_id, &attr, stream_flags, nullptr, nullptr);

//    while (true)
//    {
//        int r = pa_stream_get_state(streamOut);
//        if (r == PA_STREAM_READY)
//            break;
//        else if (r == PA_STREAM_FAILED)
//        {
//            qDebug() << "PA_STREAM_FAILED";
//            return 0;
//        }
//        pa_threaded_mainloop_wait(mloop);
//    }
//    pa_stream_cork(streamOut, 0, 0, mloop);
//    pa_threaded_mainloop_unlock(mloop); //




    pa_stream_cork(streamOut, 1, 0, mloop);

    pa_stream_disconnect(streamOut);
    pa_stream_unref(streamOut);

    pa_context_disconnect(ctx);
    pa_context_unref(ctx);

//    pa_context_disconnect(ctxRead);
//    pa_context_unref(ctxRead);
//    pa_context_disconnect(ctxWrite);
//    pa_context_unref(ctxWrite);
    pa_threaded_mainloop_stop(mloop); //
    pa_threaded_mainloop_free(mloop); //

    return 0;
}
