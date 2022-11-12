#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <pulse/pulseaudio.h>
#include <vector>


std::vector<uint32_t*> vecData;

pa_threaded_mainloop *mloop = pa_threaded_mainloop_new();

//  � ��� ��� �������� ���� ������� ��������� ������ onstatechange. ������ ������: �� ������ ������������� ������
//  ������ � ������ �� pa_threaded_mainloop_wait(), ��� �� � ������ ������ ��������. �������� ��������, ��� ��� �������
//  ���������� �� �� ������ ������������ ������ (�� ��� ��� ���������� ��������), � �� ������ �����-������, ������� ��
//  ��������� ����� � ������� pa_threaded_mainloop_start(). ��� �������, ����������, ����� ��� � ���� �������� ���������
//  ������ ��� ��� ����� ������. ���� ������� ����������, �� ��������� ��������� � ����������� ������ ������ ������ -
//  - ����� ������ ���� ����������.
void on_state_change(pa_context *c, void *userdata)
{
    pa_threaded_mainloop_signal(mloop, 0);
    qDebug() << "CONNECT";
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


void on_io_complete(pa_stream *s, size_t nbytes, void *udata)
{
    pa_threaded_mainloop_signal(mloop, 0);
    qDebug() << "on_io_complete";
}


void on_op_complete(pa_stream *s, int success, void *udata)
{
    pa_threaded_mainloop_signal(mloop, 0);
    qDebug() << "on_op_complete";
}

int main3(int argc, char *argv[])
{
   // QApplication a(argc, argv);


//  �� �������� � �������� ���������� ������, ������� ����� ������������ �������� �����-������ ������ ��� ���.
//  �� �������� ���������� ���� ����� � ������� ��� ����������� ����� ����, ��� �� �������� � PulseAudio.

    pa_threaded_mainloop_start(mloop);


//  ������, ��� ����� ������� ��� ������������� PulseAudio, - ��� ��, ��� �� ������ ��������� ��� ��������,
//  ��������� ���������� ���������� ��� ����� ������ �����-������. "������������ �����",
//  ��������� ����������� ������ �������� PA, � ����� "������������� �����".
    pa_threaded_mainloop_lock(mloop);//--------------------

//  ������ ������� ����������� � ������� PA. �������� ��������, ��� ������� pa_context_connect() ������ ����������
//  ��������� ����������, ���� ���� ���������� ��� �� �����������. �� ������� ��������� ����������� �����
//  � ������� ��������� ������, ������� �� ������ ����� pa_context_set_state_callback(). �� �������� �����������
//  �� �������, ����� �� ��������.
    pa_mainloop_api *mlapi = pa_threaded_mainloop_get_api(mloop);
    pa_context *ctx = pa_context_new_with_proplist(mlapi, "My App", NULL);

    void *udata = NULL;
    pa_context_set_state_callback(ctx, on_state_change, udata);
    pa_context_connect(ctx, NULL, pa_context_flags_t(0), NULL);

//  ����� ����, ��� �� ������ ������� �����������, ��� ������ ������ ������, ����� ��� ����� ����������. �� ����������� ������ ����������
//  � ���� ��� ��� �� ������, �� �������� pa_threaded_mainloop_wait(), ������� ��������� ��� ����� �� ��������� �������.
    while (PA_CONTEXT_READY != pa_context_get_state(ctx))
    {
        pa_threaded_mainloop_wait(mloop);
    }
    qDebug() << "here";
//  ����� ������������ ���������� � �������� PA �� ��������� � ������������ ��������� ���������. �� ������� ����� �������� � �������� ��������� ������.
//  �� ����� ����� �������� ��������� ��������� �� ���� ������� ��������� ������, �� � ������ ��������� ������� ��������. �� �������� ���������
//  ��������� ����� ���������� ��������. �, ������� ��, ���� ��� ������ ����������� ������ ��� ��������� ���������� ������ ��������� �����.

    pa_operation *op;
    void *udata1 = NULL;
    if (1) // playback
        op = pa_context_get_sink_info_list(ctx, on_dev_sink, udata1);
    else
        op = pa_context_get_source_info_list(ctx, on_dev_source, udata1);

    //  ������ ���������, ���� �������� �� ����� ���������.
    for (;;)
    {
        int r = pa_operation_get_state(op);
        if (r == PA_OPERATION_DONE || r == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(mloop);
    }

//  ���� �� ���� ����������, ����� �����-������ �������� ������ � ������� � ��������� ��������� �������� ������� ����� ������� ��������� ������, ��� �� ����� ��������
//  ������ �� ���� ��������� ��� ������� ���������� ����������. ����� ��������� ������ ��� ����� ��������� ������ ���, ��������� eol ������������� ��������� ��������.
//  ����� ��� ����������, �� ������ �������� ������ � ��� �����. ������� ��� ������������ ��������� ��������������� �������� ��������� �������: void on_dev_sink
//  � ������� ��� ������������ ������������ ��������� �������� ����������: void on_dev_source
//  �������� ������ - ��� ��������, ������� �� ������������� ��� ������ pa_context_get_*_info_list(). � ����� ���� ��� ������ ����� ����, ������ ��� ��� ���������� mloop
//  �������� ����������, � ��� ������ ������ �� �����.

    pa_operation_unref(op);



//  �� ������� ����� ���������� � ������� ������� pa_stream_new(), ��������� ��� ��� �������� �����������, ��� ������ ���������� � ������ �����, ������� �� ����� ������������.

    pa_sample_spec spec;
    spec.format = PA_SAMPLE_S16LE;
    spec.rate = 48000;
    spec.channels = 1;
    pa_stream *stm = pa_stream_new(ctx, "", &spec, NULL);

//  ����� �� ���������� ��� ����� � ���������� � ������� pa_stream_connect_*(). �� ������������� ����� ������ � pa_buffer_attr::tlength � ������, � ��� ��������� ���������
//  ��������� �� ��������� (��������� ��� ��� �������� -1). �� ����� ��������� � ������� pa_stream_set_*_callback() ���� ������� ��������� ������, ������� ����� ����������
//  ������ ���, ����� ����� ����-����� ����� ��������. �� ����� ������������ �������� device_id, ���������� ��� ������������ ���������, ��� �� ����� ������������ NULL ���
//  ���������� �� ���������.
    pa_buffer_attr attr;
    memset(&attr, 0xff, sizeof(attr));

    int buffer_length_msec = 500;
    attr.tlength = spec.rate * 16/8 * spec.channels * buffer_length_msec / 1000;

//  ��� ������ ������� �� ������:
    void *udata2 = NULL;


    pa_stream_set_read_callback(stm, on_io_complete, udata2);
    const char *device_id = NULL;
    pa_stream_connect_record(stm, device_id, &attr, pa_stream_flags_t(0));

   // pa_stream_set_write_callback(stm, on_io_complete, udata2);
   // pa_stream_connect_playback(stm, device_id, &attr, pa_stream_flags_t(0), NULL, NULL);

//  ��� ������, ��� ���������� �����, ���� ���� �������� �� ����� ���������. �� ��������� ������� ��������� ������ ������ � ������� ������� pa_stream_get_state().
//  PA_STREAM_READY ��������, ��� ������ �������� �������, � �� ����� ���������� ���������� ������. PA_STREAM_FAILED ��������, ��� ��������� ������.
    for (;;)
    {
        int r = pa_stream_get_state(stm);
        if (r == PA_STREAM_READY)
            break;
        else if (r == PA_STREAM_FAILED)
        {
            qDebug() << "PA_STREAM_FAILED";
            return 0;
        }

//  ���� �� �������� ������ pa_threaded_mainloop_wait(), ���� ������� ��������� ������ on_io_complete() ����� ������� � �����-�� ������ ������ ������ �����-������.
//  ������ �� ������ �������� ������ ������ ��������� ������.
        pa_threaded_mainloop_wait(mloop);
    }

//  � ��� ������� ���������������:
//    void *udata = NULL;
//    pa_stream_set_write_callback(stm, on_io_complete, udata);
//    const char *device_id = ...;
//    pa_stream_connect_playback(stm, device_id, &attr, 0, NULL, NULL);
//    ...

//  �� �������� ������� ������ � ������������� �� PulseAudio � ������� pa_stream_peek(), � ����� ����, ��� �� �� ����������, �� ����������� ��� ������ � ������� pa_stream_drop().
//  pa_stream_peek() ���������� 0 �������, ����� ����� ����. � ���� ������ ��� �� ����� �������� pa_stream_drop(), � �� ������ ���������, ���� �� �������� ������ ������.
//  ����� ���������� ������������ ������, �� ����� data=NULL. ��� ������ ����������� ��� ���, � �� ����� ����������, ������ pa_stream_drop(), � ����� ����� pa_stream_peek().

    for (;;)
    {
        const void *data;
        size_t n;
        pa_stream_peek(stm, &data, &n);
        if (n == 0) {
            // Buffer is empty. Process more events
            qDebug() << "buffer is empty()";
            pa_threaded_mainloop_wait(mloop);
            continue;

        } else if (data == NULL && n != 0) {
            // Buffer overrun occurred
            qDebug() << "Buffer overrun occurred";

        } else {
            qDebug() << "Good data";
            vecData.emplace_back((uint32_t*)(data));
            if (vecData.size() > 50)
                break;
        }
              pa_stream_drop(stm);
    }

    pa_stream_disconnect(stm);
    //pa_stream_unref(stm);

    pa_stream_set_write_callback(stm, on_io_complete, udata2);
    pa_stream_connect_playback(stm, device_id, &attr, pa_stream_flags_t(0), NULL, NULL);

    for (;;)
    {
        int r = pa_stream_get_state(stm);
        if (r == PA_STREAM_READY)
            break;
        else if (r == PA_STREAM_FAILED)
        {
            qDebug() << "PA_STREAM_FAILED";
            return 0;
        }
        pa_threaded_mainloop_wait(mloop);
    }

    for (int i = 0; i < vecData.size();++i)
    {
            size_t n = pa_stream_writable_size(stm);
            if (n == 0)
            {
                pa_threaded_mainloop_wait(mloop);
                continue;
            }
            void *buf = vecData[i];
            pa_stream_begin_write(stm, &buf, &n);
            void *udata3 = NULL;
            pa_operation *op = pa_stream_drain(stm, on_op_complete, udata3);
            for (;;)
            {
                int r = pa_operation_get_state(op);
                if (r == PA_OPERATION_DONE || r == PA_OPERATION_CANCELLED)
                    break;
                pa_threaded_mainloop_wait(mloop);
            }
            pa_operation_unref(op);
            pa_stream_write(stm, buf, n, NULL, 0, PA_SEEK_RELATIVE);

    }

    pa_stream_disconnect(stm);
    pa_stream_unref(stm);






    pa_context_disconnect(ctx);
    pa_context_unref(ctx);

    pa_threaded_mainloop_unlock(mloop); // ������������� ������//------------------


    pa_threaded_mainloop_stop(mloop); // ��������� ������
    pa_threaded_mainloop_free(mloop); // �������� �����������


    //return a.exec();
    return 0;
}



