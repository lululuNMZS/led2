#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/cdev.h>

#define BCM2837_GPIO_BASE             0x3F200000
#define BCM2837_GPIO_FSEL0_OFFSET     0x0   // GPIO功能选择寄存器0
#define BCM2837_GPIO_FSEL1_OFFSET     0x04  // GPIO功能选择寄存器1
#define BCM2837_GPIO_FSEL2_OFFSET     0x08  // GPIO功能选择寄存器2
#define BCM2837_GPIO_SET0_OFFSET      0x1C  // GPIO置位寄存器0
#define BCM2837_GPIO_CLR0_OFFSET      0x28  // GPIO清零寄存器0

#define LED_RED_PIN 17
#define LED_GREEN_PIN 27
#define LED_BLUE_PIN 22


static  struct  {
	const char* name;
	const char pin;
}colors[]={
  { "white",  7 },  // 白(全开) 0b111
  { "black",  0 },  // 黑(全关) 0b000
  { "red",    4 },  // 红	0b100
  { "green",  2 },  // 绿 0b010
  { "blue",   1 },  // 蓝 0b001
  { "yellow", 6 },  // 黄 0b110
  { "cyan",   3 },  // 青 0b011
  { "purple", 5 },  // 紫 0b101
},led_state;


static void* gpio = 0;
static dev_t devno=0;
static struct cdev cdev;


static void rgbled_write(void){
		
}

void rgbled_read(void){

}


void rgbled_ioctl(char pin){
	char r_pin,g_pin,b_pin;
	r_pin= pin & (~(~4));
	g_pin= pin & (~(~2));
	b_pin= pin & (~(~1));

	int reg_set = gpio+BCM2837_GPIO_SET0_OFFSET;
	int reg_clr = gpio+BCM2837_GPIO_CLR0_OFFSET;
	if (r_pin != 0)
		iowrite32(1<<LED_RED_PIN,reg_set);
	else
		iowrite32(1<<LED_RED_PIN,reg_clr);
	if (g_pin != 0)
		iowrite32(1<<LED_GREEN_PIN,reg_set);
	else
		iowrite32(1<<LED_GREEN_PIN,reg_clr);
	if (b_pin != 0)
		iowrite32(1<<LED_BLUE_PIN,reg_set);
	else
		iowrite32(1<<LED_BLUE_PIN,reg_clr);

	//led_state.name=
	led_state.pin=pin;
}


static const struct file_operations fops = {
	.owner=THIS_MODULE,
	.read=rgbled_read,
	.write=rgbled_write,
	.unlocked_ioctl=rgbled_ioctl,
};


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

  rgbled_ioctl(colors[5].pin);

  return 0;
}
module_init(rgbled_init);


static void __exit rgbled_exit(void)
{
  // GPIO输出0
  iowrite32(1 << 17, gpio + BCM2837_GPIO_CLR0_OFFSET);
  iowrite32(1 << 27, gpio + BCM2837_GPIO_CLR0_OFFSET);
  iowrite32(1 << 22, gpio + BCM2837_GPIO_CLR0_OFFSET);
  printk(KERN_INFO"Bye led2");
  iounmap(gpio);

}
module_exit(rgbled_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | ixx.life");
