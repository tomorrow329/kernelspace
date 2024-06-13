#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/io.h>
#include <linux/device.h>

int major;

struct class *cls;
struct device *dev;

static char *kernel_buffer;
static size_t buffer_size = 1024;


/*封装操作方法*/
int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

ssize_t mycdev_read(struct file *file, char *ubuf, size_t size, loff_t *lof)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);

	int ret;
	if(size>buffer_size)
		size=buffer_size;

	ret=copy_to_user(ubuf, kernel_buffer, size);
	if(ret)
	{
		printk("copy_to_user failed\n");
		return ret;
	}
	printk("copy_to_user successful\n");

	return 0;
}

ssize_t mycdev_write(struct file *file, const char *ubuf, size_t size, loff_t *lof)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	
	int ret;
	if(size>buffer_size)
		size=buffer_size;

	ret=copy_from_user(kernel_buffer, ubuf, size);
	if(ret)
	{
		printk("copy_from_user failed\n");
		return ret;
	}
	printk("copy_from_user successful\n");

	return 0;
}

int mycdev_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

/*定义操作方法结构体遍历并且初始化*/
struct file_operations fops = {
	.open = mycdev_open,
	.read = mycdev_read,
	.write= mycdev_write,
	.release = mycdev_close,
};

/*入口函数*/
static int __init mycdev_init(void)
{
	//申请指定大小的内存空间
	kernel_buffer = kmalloc(buffer_size, GFP_KERNEL);
	if(!kernel_buffer)
	{
		printk("申请空间失败");
		return -ENOMEM;
	}
	printk("申请空间成功");

	//注册字符设备驱动
	major = register_chrdev(0, "mycdev", &fops);
	if(major < 0)
	{
		printk("字符设备驱动注册失败\n");
		return major;
	}
	printk("字符设备驱动注册成功, major=%d\n", major);

	//向上提交目录信息
	cls=class_create(THIS_MODULE, "mycdev");
	if(IS_ERR(cls))
	{
		printk("向上提交目录信息失败\n");
		return -PTR_ERR(cls);
	}
	printk("向上提交目录信息成功\n");

	//向上提交设备信息
	dev=device_create(cls, NULL, MKDEV(major, 0), NULL, "mycdev0");
	if(IS_ERR(dev))
	{
		printk("向上提交设备节点失败\n");
		return -PTR_ERR(dev);
	}
	printk("向上提交设备节点成功\n");


	return 0;
}

/*出口函数*/
static void __exit mycdev_exit(void)
{

	//销毁设备信息
	device_destroy(cls, MKDEV(major, 0));

	//销毁目录信息
	class_destroy(cls);

	//字符设备驱动注销
	unregister_chrdev(major, "mycdev");

	//释放申请的空间
	kfree(kernel_buffer);

}

module_init(mycdev_init);
module_exit(mycdev_exit);
/*用于声明当前内核模块遵循GPL协议*/
MODULE_LICENSE("GPL");

