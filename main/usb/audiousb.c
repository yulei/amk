#include "audiousb.h"
#include "usbd_composite.h"

static int8_t uac_init(uint32_t AudioFreq, uint32_t Volume, uint32_t options)
{
    return (USBD_OK);
}

static int8_t uac_uninit(uint32_t options)
{
    return (USBD_OK);
}


static int8_t uac_command(uint8_t* pbuf, uint32_t size, uint8_t cmd)
{
    switch(cmd)
    {
      case AUDIO_CMD_START:
      break;

      case AUDIO_CMD_PLAY:
      break;
    }
    return (USBD_OK);
}

static int8_t uac_volume_control(uint8_t vol)
{
    return (USBD_OK);
}

static int8_t uac_mute_control(uint8_t cmd)
{
    return (USBD_OK);
}

static int8_t uac_periodic_tc(uint8_t *pbuf, uint32_t size, uint8_t cmd)
{
    return (USBD_OK);
}

static int8_t uac_get_state(void)
{
    return (USBD_OK);
}

USBD_AUDIO_ItfTypeDef uac_ops =
{
    uac_init,
    uac_uninit,
    uac_audio_command,
    uac_volume_control,
    uac_mute_control,
    uac_peridic_tc,
    uac_get_state,
};

void TransferComplete_CallBack_FS(void)
{
    //USBD_AUDIO_Sync(&hUsbDeviceFS, AUDIO_OFFSET_FULL);
}

void HalfTransfer_CallBack_FS(void)
{
    //USBD_AUDIO_Sync(&hUsbDeviceFS, AUDIO_OFFSET_HALF);
}
