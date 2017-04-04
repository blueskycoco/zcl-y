#include <stdint.h>
#include <rtdevice.h>
#include "board.h"
#include "nand-elm.h"

#define NAND_DEBUG

#ifdef NAND_DEBUG
#define NAND_TRACE         rt_kprintf
#else
#define NAND_TRACE(...)
#endif
static struct rt_mutex lock;
static struct rt_device flash_device;

static void flash_lock(void)
{
    rt_mutex_take(&lock, RT_WAITING_FOREVER);
}

static void flash_unlock(void)
{
    rt_mutex_release(&lock);
}

static rt_err_t nand_flash_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t nand_flash_open(rt_device_t dev, rt_uint16_t oflag)
{    
    return RT_EOK;
}

static rt_err_t nand_flash_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t nand_flash_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = 2048;
        geometry->block_size = 64 * geometry->bytes_per_sector;
       	geometry->sector_count = 1024;//4096
	}
	return RT_EOK;
}

static rt_size_t nand_flash_read(rt_device_t dev,
                                   rt_off_t pos,
                                   void* buffer,
                                   rt_size_t size)
{
   
    flash_lock();
   	unsigned char result;
    flash_unlock();
    //if(result == MED_STATUS_SUCCESS)
    	return size;

	return 0;
}

static rt_size_t nand_flash_write(rt_device_t dev,
                                    rt_off_t pos,
                                    const void* buffer,
                                    rt_size_t size)
{ 
 
    flash_lock();
	unsigned int result;	 
    flash_unlock();

	//if(result == MED_STATUS_SUCCESS)
    	return size;

	return 0;
}

rt_int32_t rt_hw_nand_init(void)
{
	rt_int32_t stat = RT_EOK;
    /* initialize mutex */
    if (rt_mutex_init(&lock, "nand_mutex", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("init sd lock mutex failed\n");
        return -RT_ENOSYS;
    }
    
    /* register device */
    flash_device.type    = RT_Device_Class_Block;
    flash_device.init    = nand_flash_init;
    flash_device.open    = nand_flash_open;
    flash_device.close   = nand_flash_close;
    flash_device.read    = nand_flash_read;
    flash_device.write   = nand_flash_write;
    flash_device.control = nand_flash_control;
    /* no private */
    flash_device.user_data = RT_NULL;

    rt_device_register(&flash_device, "nand0",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);

    return stat;
}
