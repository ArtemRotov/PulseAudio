#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <pulse/pulseaudio.h>
#include <vector>


std::vector<uint32_t*> vecData;

pa_threaded_mainloop *mloop = pa_threaded_mainloop_new();

//  А вот как выглядит наша функция обратного вызова onstatechange. Ничего умного: мы просто сигнализируем нашему
//  потоку о выходе из pa_threaded_mainloop_wait(), где мы в данный момент зависаем. Обратите внимание, что эта функция
//  вызывается не из нашего собственного потока (он все еще продолжает зависать), а из потока ввода-вывода, который мы
//  запустили ранее с помощью pa_threaded_mainloop_start(). Как правило, старайтесь, чтобы код в этих функциях обратного
//  вызова был как можно меньше. Ваша функция вызывается, вы получаете результат и отправляете сигнал своему потоку -
//  - этого должно быть достаточно.
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


//  Мы начинаем с создания отдельного потока, который будет обрабатывать операции ввода-вывода сокета для нас.
//  Не забудьте остановить этот поток и закрыть его обработчики после того, как мы закончим с PulseAudio.

    pa_threaded_mainloop_start(mloop);


//  Первое, что нужно помнить при использовании PulseAudio, - это то, что мы должны выполнять все операции,
//  удерживая внутреннюю блокировку для этого потока ввода-вывода. "Заблокируйте поток",
//  выполните необходимые вызовы объектов PA, а затем "разблокируйте поток".
    pa_threaded_mainloop_lock(mloop);//--------------------

//  Теперь начните подключение к серверу PA. Обратите внимание, что функция pa_context_connect() обычно возвращает
//  результат немедленно, даже если соединение еще не установлено. Мы получим результат подключения позже
//  в функции обратного вызова, которую мы задаем через pa_context_set_state_callback(). Не забудьте отключиться
//  от сервера, когда мы закончим.
    pa_mainloop_api *mlapi = pa_threaded_mainloop_get_api(mloop);
    pa_context *ctx = pa_context_new_with_proplist(mlapi, "My App", NULL);

    void *udata = NULL;
    pa_context_set_state_callback(ctx, on_state_change, udata);
    pa_context_connect(ctx, NULL, pa_context_flags_t(0), NULL);

//  После того, как мы выдали команду подключения, нам больше нечего делать, кроме как ждать результата. Мы запрашиваем статус соединения
//  и если оно еще не готово, мы вызываем pa_threaded_mainloop_wait(), который блокирует ваш поток до получения сигнала.
    while (PA_CONTEXT_READY != pa_context_get_state(ctx))
    {
        pa_threaded_mainloop_wait(mloop);
    }
    qDebug() << "here";
//  После установления соединения с сервером PA мы переходим к перечислению доступных устройств. Мы создаем новую операцию с функцией обратного вызова.
//  Мы также можем передать некоторый указатель на нашу функцию обратного вызова, но я просто использую НУЛЕВОЕ значение. Не забудьте отпустить
//  указатель после завершения операции. И, конечно же, этот код должен выполняться только при удержании блокировки потока основного цикла.

    pa_operation *op;
    void *udata1 = NULL;
    if (1) // playback
        op = pa_context_get_sink_info_list(ctx, on_dev_sink, udata1);
    else
        op = pa_context_get_source_info_list(ctx, on_dev_source, udata1);

    //  Теперь подождите, пока операция не будет завершена.
    for (;;)
    {
        int r = pa_operation_get_state(op);
        if (r == PA_OPERATION_DONE || r == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(mloop);
    }

//  Пока мы этим занимаемся, поток ввода-вывода получает данные с сервера и выполняет несколько успешных вызовов нашей функции обратного вызова, где мы можем получить
//  доступ ко всем свойствам для каждого доступного устройства. Когда произошла ошибка или когда устройств больше нет, параметру eol присваивается ненулевое значение.
//  Когда это происходит, мы просто посылаем сигнал в наш поток. Функция для перечисления устройств воспроизведения выглядит следующим образом: void on_dev_sink
//  И функция для перечисления записывающих устройств выглядит аналогично: void on_dev_source
//  Значение данных - это значение, которое мы устанавливаем при вызове pa_context_get_*_info_list(). В нашем коде они всегда равны нулю, потому что моя переменная mloop
//  является глобальной, и нам больше ничего не нужно.

    pa_operation_unref(op);



//  Мы создаем новый аудиобуфер с помощью функции pa_stream_new(), передавая ему наш контекст подключения, имя нашего приложения и формат звука, который мы хотим использовать.

    pa_sample_spec spec;
    spec.format = PA_SAMPLE_S16LE;
    spec.rate = 48000;
    spec.channels = 1;
    pa_stream *stm = pa_stream_new(ctx, "", &spec, NULL);

//  Далее мы подключаем наш буфер к устройству с помощью pa_stream_connect_*(). Мы устанавливаем длину буфера в pa_buffer_attr::tlength в байтах, а все остальные параметры
//  оставляем по умолчанию (установив для них значение -1). Мы также назначаем с помощью pa_stream_set_*_callback() нашу функцию обратного вызова, которая будет вызываться
//  каждый раз, когда аудио ввод-вывод будет завершен. Мы можем использовать значение device_id, полученное при перечислении устройств, или мы можем использовать NULL для
//  устройства по умолчанию.
    pa_buffer_attr attr;
    memset(&attr, 0xff, sizeof(attr));

    int buffer_length_msec = 500;
    attr.tlength = spec.rate * 16/8 * spec.channels * buffer_length_msec / 1000;

//  Для записи потоков мы делаем:
    void *udata2 = NULL;


    pa_stream_set_read_callback(stm, on_io_complete, udata2);
    const char *device_id = NULL;
    pa_stream_connect_record(stm, device_id, &attr, pa_stream_flags_t(0));

   // pa_stream_set_write_callback(stm, on_io_complete, udata2);
   // pa_stream_connect_playback(stm, device_id, &attr, pa_stream_flags_t(0), NULL, NULL);

//  Как обычно, нам приходится ждать, пока наша операция не будет завершена. Мы считываем текущее состояние нашего буфера с помощью функции pa_stream_get_state().
//  PA_STREAM_READY означает, что запись запущена успешно, и мы можем продолжить нормальную работу. PA_STREAM_FAILED означает, что произошла ошибка.
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

//  Пока мы зависаем внутри pa_threaded_mainloop_wait(), наша функция обратного вызова on_io_complete() будет вызвана в какой-то момент внутри потока ввода-вывода.
//  Теперь мы просто посылаем сигнал нашему основному потоку.
        pa_threaded_mainloop_wait(mloop);
    }

//  И для потоков воспроизведения:
//    void *udata = NULL;
//    pa_stream_set_write_callback(stm, on_io_complete, udata);
//    const char *device_id = ...;
//    pa_stream_connect_playback(stm, device_id, &attr, 0, NULL, NULL);
//    ...

//  Мы получаем область данных с аудиосэмплами из PulseAudio с помощью pa_stream_peek(), и после того, как мы ее обработали, мы отбрасываем эти данные с помощью pa_stream_drop().
//  pa_stream_peek() возвращает 0 выборок, когда буфер пуст. В этом случае нам не нужно вызывать pa_stream_drop(), и мы должны подождать, пока не поступит больше данных.
//  Когда происходит переполнение буфера, мы имеем data=NULL. Это просто уведомление для нас, и мы можем продолжить, вызвав pa_stream_drop(), а затем снова pa_stream_peek().

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

    pa_threaded_mainloop_unlock(mloop); // Разблокировка потока//------------------


    pa_threaded_mainloop_stop(mloop); // Остановка потока
    pa_threaded_mainloop_free(mloop); // Закрытие обработчика


    //return a.exec();
    return 0;
}



