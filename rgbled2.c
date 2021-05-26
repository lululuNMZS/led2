#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/delay.h>

#define BCM2837_GPIO_BASE             0x3F200000
#define BCM2837_GPIO_FSEL0_OFFSET     0x0   // GPIO功能选择寄存器0
#define BCM2837_GPIO_FSEL1_OFFSET     0x04  // GPIO功能选择寄存器1
#define BCM2837_GPIO_FSEL2_OFFSET     0x08  // GPIO功能选择寄存器2
#define BCM2837_GPIO_SET0_OFFSET      0x1C  // GPIO置位寄存器0
#define BCM2837_GPIO_CLR0_OFFSET      0x28  // GPIO清零寄存器0

// 三色LED灯不同状态组合
static struct { const char* name; const bool pins[3]; } colors[] = {
  { "white",  {1,1,1} },  // 白(全开)
  { "black",  {0,0,0} },  // 黑(全关)
  { "red",    {1,0,0} },  // 红
  { "green",  {0,1,0} },  // 绿
  { "blue",   {0,0,1} },  // 蓝
  { "yellow", {1,1,0} },  // 黄
  { "cyan",   {0,1,1} },  // 青
  { "purple", {1,0,1} },  // 紫
};


static void* gpio = 0;
static dev_t devno=0;
static struct cdev cdev;

/*
static const struct file_operations fops = {
	.owner=THIS_MODULE,
	.read=rgbled_read,
	.write=rgbled_write,
	.unlocked_ioctl=rgbled_ioctl,
};
*/

static int __init rgbled_init(void)
{
  // 获取GPIO对应的Linux虚拟内存地址
  gpio = ioremap(BCM2837_GPIO_BASE, 0xB0);

  // 将GPIO1 bit17设置为“输出模式”
  int val = ioread32(gpio + BCM2837_GPIO_FSEL1_OFFSET);
  val &= ~(7 << 21);
  val |= 1 << 21;
  iowrite32(val, gpio+BCM2837_GPIO_FSEL1_OFFSET);


  // 将GPIO2 bit22 bit27设置为“输出模式”
  int val_2 = ioread32(gpio + BCM2837_GPIO_FSEL2_OFFSET);
  val_2 &= ~(7 << 6);
  val_2 |= 1 << 6;
  iowrite32(val_2, gpio+BCM2837_GPIO_FSEL2_OFFSET);

  val_2 &= ~(7 << 21);
  val_2 |= 1 << 21;
  iowrite32(val_2, gpio+BCM2837_GPIO_FSEL2_OFFSET);

  //将该模块注册为字符设备，动态分配设备号
  if(alloc_chrdev_region(&devno,0,1,"rgbled2")){
	  printk(KERN_ERR"Failed to register kernel module!\n");
	  return -1;
  }
  cdev_init(&cdev,&fops);
  cdev_add(&cdev,devno,1);

  // GPIO bit2 输出1
  mdelay(2000);
  iowrite32(1 << 27, gpio + BCM2837_GPIO_SET0_OFFSET);
  mdelay(2000);
  iowrite32(1 << 27, gpio + BCM2837_GPIO_CLR0_OFFSET);
  mdelay(2000);
  iowrite32(1 << 17, gpio + BCM2837_GPIO_SET0_OFFSET);
  mdelay(2000);
  iowrite32(1 << 17, gpio + BCM2837_GPIO_CLR0_OFFSET);
  mdelay(2000);
  iowrite32(1 << 22, gpio + BCM2837_GPIO_SET0_OFFSET);
  mdelay(2000);
  iowrite32(1 << 22, gpio + BCM2837_GPIO_CLR0_OFFSET);
  return 0;
}
module_init(rgbled_init);

static void __exit rgbled_exit(void)
{
  // GPIO输出0
  iowrite32(1 << 27, gpio + BCM2837_GPIO_CLR0_OFFSET);
  iounmap(gpio);

}
module_exit(rgbled_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | ixx.life");
