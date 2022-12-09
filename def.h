#pragma once
#include <pulse/def.h>

class pa_threaded_mainloop;
class pa_mainloop_api;
class pa_context;
class pa_operation;
class pa_channel_map;
class pa_sample_spec;
class pa_buffer_attr;
class pa_stream;

namespace pulse
{
    const char* const BasicDevice = nullptr;

    void* const NullData = nullptr;


    const pa_stream_flags_t PlaybStreamFlags = pa_stream_flags_t(PA_STREAM_START_CORKED |
    PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_NOT_MONOTONIC | PA_STREAM_AUTO_TIMING_UPDATE | PA_STREAM_ADJUST_LATENCY);

    const pa_stream_flags_t RecStreamFlags = pa_stream_flags_t(PA_STREAM_ADJUST_LATENCY | PA_STREAM_START_MUTED);


    class SampleSpecification;

    class BufferAttributes;


    typedef     pa_threaded_mainloop*   MainLoopPtr;

    typedef     pa_mainloop_api*        MainLoopApiPtr;

    typedef     pa_context*             ContextPtr;

    typedef     pa_operation*           OperationPtr;

    typedef     pa_buffer_attr*         BufferAttrPtr;

    typedef     pa_sample_spec*         SampleSpecPtr;

    typedef     pa_channel_map          ChannelMap;

    typedef     pa_channel_map*         ChannelMapPtr;

    typedef     pa_stream*              StreamPtr;
}
