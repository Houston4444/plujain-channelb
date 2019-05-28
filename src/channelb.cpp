#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lv2.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://plujain/plugins/channelb"
enum {IN, OUT_1, OUT_2, CHANNEL, CROSS_LENGTH_AB, UNIT_AB, SHAPE_AB,
      CROSS_LENGTH_BA, UNIT_BA, SHAPE_BA, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class ChannelB
{
public:
    ChannelB() {}
    ~ChannelB() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *in;
    float *out_1;
    float *out_2;
    float *asked_channel;
    float *cross_length_ab;
    float *cross_length_ba;
    float *unit_ab;
    float *unit_ba;
    float *shape_ab;
    float *shape_ba;
    float channel_ratio;
    double samplerate;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    ChannelB::instantiate,
    ChannelB::connect_port,
    ChannelB::activate,
    ChannelB::run,
    ChannelB::deactivate,
    ChannelB::cleanup,
    ChannelB::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle ChannelB::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    ChannelB *plugin = new ChannelB();
    
    plugin->channel_ratio = 0.00f;
    plugin->samplerate = samplerate;
    
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void ChannelB::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void ChannelB::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}
        

/**********************************************************************************************************************************************************/

void ChannelB::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    ChannelB *plugin;
    plugin = (ChannelB *) instance;

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT_1:
            plugin->out_1 = (float*) data;
            break;
        case OUT_2:
            plugin->out_2 = (float*) data;
            break;
        case CHANNEL:
            plugin->asked_channel = (float*) data;
//             plugin->gc_last_sw = plugin->gain_channel;
            break;
        case CROSS_LENGTH_AB:
            plugin->cross_length_ab = (float*) data;
            break;
        case UNIT_AB:
            plugin->unit_ab = (float*) data;
            break;
        case SHAPE_AB:
            plugin->shape_ab = (float*) data;
            break;
        case CROSS_LENGTH_BA:
            plugin->cross_length_ba = (float*) data;
            break;
        case UNIT_BA:
            plugin->unit_ba = (float*) data;
            break;
        case SHAPE_BA:
            plugin->shape_ba = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void ChannelB::run(LV2_Handle instance, uint32_t n_samples)
{
    ChannelB *plugin;
    plugin = (ChannelB *) instance;
    
    int canal;
    canal = (int)(*(plugin->asked_channel));
    
    float gain_channel_1 = 0.00f;
    float gain_channel_2 = 0.00f;
    
    switch (canal){
        default:
        case 0:
            for ( uint32_t i = 0; i < n_samples; i++)
            {
                if (plugin->channel_ratio == 0.0f){
                    plugin->out_1[i] = plugin->in[i];
                    plugin->out_2[i] = 0.0f;
                    continue;
                }
                
                float cross_length_ba = *plugin->cross_length_ba;
                if (*plugin->unit_ba != 1.00f){
                    cross_length_ba = cross_length_ba / 1000;
                }
                
                plugin->channel_ratio -= 1/(cross_length_ba * plugin->samplerate);
                
                if (plugin->channel_ratio <= 0.00f){
                    plugin->channel_ratio = 0.00f;
                }
                
                float ratio = plugin->channel_ratio;
                float aratio = 1 - ratio;
                
                if (*plugin->shape_ba < 0.0f){
                    gain_channel_1 = aratio - (sin(M_PI_2 * (aratio -1)) +1 - aratio) * *plugin->shape_ba;
                    gain_channel_2 = 1.0f - gain_channel_1;
                } else {
                    gain_channel_1 = aratio + (sin(M_PI_2 * aratio) - aratio) * *plugin->shape_ba;
                    gain_channel_2 = 1.0f - gain_channel_1;
                }
                
                plugin->out_1[i] = plugin->in[i] * gain_channel_1;
                plugin->out_2[i] = plugin->in[i] * gain_channel_2;
            }
        
        break ;
        
        case 1:
            for ( uint32_t i = 0; i < n_samples; i++)
            {
                if (plugin->channel_ratio == 1.0f){
                    plugin->out_1[i] = 0.0f;
                    plugin->out_2[i] = plugin->in[i];
                    continue;
                }
                
                float cross_length_ab = *plugin->cross_length_ab;
                if (*plugin->unit_ab != 1.0f){
                    cross_length_ab = cross_length_ab / 1000;
                }
                
                plugin->channel_ratio += 1/(cross_length_ab * plugin->samplerate);
                if (plugin->channel_ratio >= 1.00f){
                    plugin->channel_ratio = 1.00f;
                }
                
                float ratio = plugin->channel_ratio;
                
                if (*plugin->shape_ab < 0.0f){
                    gain_channel_2 = ratio  - (sin(M_PI_2 * (ratio  -1)) +1 - ratio ) * *plugin->shape_ab;
                    gain_channel_1 = 1.0f - gain_channel_2;
                } else {
                    gain_channel_2 = ratio  + (sin(M_PI_2 * ratio ) - ratio ) * *plugin->shape_ab;
                    gain_channel_1 = 1.0f - gain_channel_2;
                }
                
                plugin->out_1[i] = plugin->in[i] * gain_channel_1;
                plugin->out_2[i] = plugin->in[i] * gain_channel_2;
            }
        break ;
    }
}

/**********************************************************************************************************************************************************/

void ChannelB::cleanup(LV2_Handle instance)
{
    delete ((ChannelB *) instance);
}

/**********************************************************************************************************************************************************/

const void* ChannelB::extension_data(const char* uri)
{
    return NULL;
}
