#include "Gui/mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <vector>
#include <queue>
#include <QThread>
#include <mutex>
#include <pulse/pulseaudio.h>

#define RATE 48000

pa_threaded_mainloop *mloop = pa_threaded_mainloop_new();
std::mutex mut;
std::queue<uint8_t> vecData;

void stream_state_cb(pa_stream *s, void *mainloop);

void on_state_change(pa_context *context, void *userdata)
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
    std::lock_guard<std::mutex> lock (mut);
    static int i = 1;
    qDebug() << "write call " << i++ ;

    size_t bytes_remaining = requested_bytes;

    while (bytes_remaining > 0)
    {
        uint8_t *buffer = nullptr;
        size_t bytes_to_fill = RATE;
        if (bytes_to_fill > bytes_remaining) bytes_to_fill = bytes_remaining;
        pa_stream_begin_write(stream, (void**) &buffer, &bytes_to_fill);

        for (size_t i = 0; i < bytes_to_fill; ++i)
        {
            if (!vecData.empty())
            {
                buffer[i] = vecData.front();
                vecData.pop();
            }
            else
                buffer[i] = 0x0;
        }
        //TODO pa_xfree (pulse/xmalloc);
        //pa_stream_write(stream, buffer, bytes_to_fill, pa_xfree, 0, PA_SEEK_RELATIVE);
        pa_stream_write(stream, buffer, bytes_to_fill, NULL, 0, PA_SEEK_RELATIVE);
        bytes_remaining -= bytes_to_fill;
    }
    //qDebug() << vecData.size();
}

void on_i_complete(pa_stream *stream, size_t nbytes, void *udata)
{
    std::lock_guard<std::mutex> lock (mut);
    static int i = 1;
    qDebug() << "read call " << i++ ;
    while (true)
    {
        const void* data;
        size_t n;
        pa_stream_peek(stream, &data, &n);
        if (data == NULL && n == 0) {
            // Buffer is empty. Process more events
            break;
        } else if (data == NULL && n != 0) {
            // Buffer overrun occurred
            qDebug() << "Buffer overrun occurred";
            return;

        } else {
            uint8_t* ptr = (uint8_t*)data;
            for (int i = 0; i < n; ++i)
            {
                if (ptr[i] != 0x0)
                    vecData.push(ptr[i]);
            }

            //qDebug() << vecData.size();
        }
              pa_stream_drop(stream);
    }
    //qDebug() << vecData.size();
}


void on_op_complete(pa_stream *s, int success, void *udata)
{
    pa_threaded_mainloop_signal(mloop, 0);
    qDebug() << "on_op_complete";
}

void stream_state_cb(pa_stream *s, void *mainloop)
{
    pa_threaded_mainloop_signal((pa_threaded_mainloop*)mainloop, 0);
}


int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    pa_threaded_mainloop_lock(mloop);
    pa_threaded_mainloop_start(mloop);
    pa_mainloop_api *mlapi = pa_threaded_mainloop_get_api(mloop);
    pa_context *ctx = pa_context_new_with_proplist(mlapi, "MyAudioProject", nullptr);

    void *udata = nullptr;
    pa_context_set_state_callback(ctx, on_state_change, udata);
    pa_context_connect(ctx, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
    pa_threaded_mainloop_wait(mloop); //wait connect

    pa_operation *operationSink;
    pa_operation *operationSource;
    void* udataSink = nullptr;
    void* udataSource = nullptr;

    operationSink = pa_context_get_sink_info_list(ctx, on_dev_sink, udataSink);
    while (true)
    {
        int result = pa_operation_get_state(operationSink);
        if (result == PA_OPERATION_DONE || result == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(mloop);
    }
    pa_operation_unref(operationSink);

    operationSource = pa_context_get_source_info_list(ctx, on_dev_source, udataSource);
    while (true)
    {
        int result = pa_operation_get_state(operationSource);
        if (result == PA_OPERATION_DONE || result == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(mloop);
    }
    pa_operation_unref(operationSource);



    // [1] Настройка каналов
    pa_sample_spec spec;
    spec.format = PA_SAMPLE_U8;
    spec.rate = RATE;
    spec.channels = 2;
    pa_channel_map map;     //Тут смотреть левый правый похоже
    pa_channel_map_init_stereo(&map);
    map.map[0] = PA_CHANNEL_POSITION_FRONT_LEFT;   //PA_CHANNEL_POSITION_FRONT_LEFT
    map.map[1] = PA_CHANNEL_POSITION_FRONT_RIGHT;    //PA_CHANNEL_POSITION_FRONT_RIGHT

    pa_stream *stream = pa_stream_new(ctx, "MyAudioProject", &spec, &map);
    pa_buffer_attr attr;
    attr.maxlength = (uint32_t) -1;
    attr.tlength = (uint32_t) -1;
    attr.prebuf = (uint32_t) -1;
    attr.minreq = (uint32_t) -1;
    attr.fragsize = (uint32_t) -1;

    pa_stream_flags_t stream_flags = pa_stream_flags_t(PA_STREAM_START_CORKED | PA_STREAM_INTERPOLATE_TIMING |
    PA_STREAM_NOT_MONOTONIC | PA_STREAM_AUTO_TIMING_UPDATE |
    PA_STREAM_ADJUST_LATENCY);

    // [1]

    void* dataRecord = nullptr;
    const char *device_id = nullptr;
    pa_stream_set_state_callback(stream, stream_state_cb, mloop);
    pa_stream_set_read_callback(stream, on_i_complete, dataRecord);
    if (pa_stream_connect_record(stream, device_id, &attr, PA_STREAM_NOFLAGS) != 0)
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
    //pa_stream_flush(stream,0,nullptr);
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

    pa_stream *streamOut = pa_stream_new(ctx, "MyAudioProjectOut", &spec, &map);
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

    qDebug() << "START WORKING";
    pa_stream_cork(streamOut, 0, 0, mloop);
    pa_stream_trigger(streamOut,NULL,NULL);
    while(true)
    {

    }
    qDebug() << "STOP WORKING";
    pa_stream_cork(streamOut, 1, 0, mloop);
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


    QThread::sleep(6);

    pa_stream_cork(streamOut, 1, 0, mloop);

    pa_stream_disconnect(streamOut);
    pa_stream_unref(streamOut);

    pa_context_disconnect(ctx);
    pa_context_unref(ctx);
    pa_threaded_mainloop_stop(mloop); //
    pa_threaded_mainloop_free(mloop); //
    return 0;
}













