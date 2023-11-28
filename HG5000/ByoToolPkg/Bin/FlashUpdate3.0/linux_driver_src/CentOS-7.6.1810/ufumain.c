#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "ufumain.h"
#include <asm/device.h>
#include <linux/device.h>


struct cdev ufu_dev;
dev_t devno;
struct class *cdev_class;

int
ufu_open (struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "ufudev open\n");
  return 0;
}

int
ufu_release(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "ufudev close\n");
  return 0;
}

static long
ufu_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
  long                    ret  = 0x0;
  size_t                  size = 0x0;
  void                    *addr;
  UFU_SMI_BUFFER           smi;
  long                     smiret;
  UFU_MEM_OPERATION_BUFFER mem;
  long      func, faddr, smiport;
 
  switch (cmd) {
    case IOCTL_UFU_MEM_ALLOCATE:
      printk(KERN_INFO "bytt, mem allocate.\n");

      copy_from_user(&mem, (void *)arg, sizeof(UFU_MEM_OPERATION_BUFFER));
      size = (size_t)mem.size;
      // alloc memory
      mem.dst = (ADDRESS)kmalloc(size, GFP_DMA);
      printk(KERN_INFO "bytt, IOCTL_UFU_MEM_ALLOCATE, alloc mem size:%d, add is: %llx\n", size, mem.dst);
      if (!mem.dst) {
        mem.dst = 0;
      }

      copy_to_user((void*)arg, &mem, sizeof(UFU_MEM_OPERATION_BUFFER));
      
      break;

    case IOCTL_UFU_MEM_FREE:
      addr = (void*)arg;
      kfree((void*)addr);
      printk(KERN_INFO "bytt, IOCTL_UFU_MEM_FREE,free address=%p\n", addr);
      ret = 0;
      break;

    case IOCTL_UFU_MEM_WRITE:
     
      copy_from_user(&mem, (void *)arg, sizeof(UFU_MEM_OPERATION_BUFFER));
      printk(KERN_INFO "bytt, IOCTL_UFU_MEM_WRITE\n");
      printk(KERN_INFO "mem->dst = 0x%llx mem->src = 0x%llx ",mem.dst,mem.src);
      ret = copy_from_user((void *)mem.dst, (void *)mem.src, mem.size);
      break;

    case IOCTL_UFU_MEM_READ:
      copy_from_user(&mem, (void *)arg, sizeof(UFU_MEM_OPERATION_BUFFER));
      printk(KERN_INFO "bytt, IOCTL_UFU_MEM_READ, src=%p, dest=%p, size=%u\n", mem.src, mem.dst, mem.size);
      ret = copy_to_user((void *)mem.dst, (void *)mem.src, mem.size);
      break;

    case IOCTL_UFU_PHYSICAL_ADDRESS:
      addr = (void*)arg;
      ret = virt_to_phys(addr);
      printk(KERN_INFO "bytt, IOCTL_UFU_PHYSICAL_ADDRESS, virt:%p --> phy:%p\n", addr, ret);
      break;

    case IOCTL_UFU_SMI_CALL:
      copy_from_user(&smi,(void*)arg,sizeof(UFU_SMI_BUFFER));
      func = (smi.func)&0xffff;
      faddr = smi.faddr;
      smiport = (smi.smiport)&0xffff;
      printk(KERN_INFO "before SMI_CALL_asm, func=%p, subfunc=%p, addr=%p, smiport=%x\n", func&0xff, func>>8, faddr, smiport);
      smiret = 0;
#ifdef __i386__
      __asm__ __volatile__( "push %%ebx;"
        "movl %[func], %%eax;"
        "movl %[faddr], %%ebx;"
        "movl %[smiport], %%edx;"
        "out %%al, %%dx;"
        "movl %%eax, %[smiret];"
        "nop;"
        "nop;"
        "pop %%ebx;":[smiret]"+c"(smiret):[func]"a"(func),[faddr]"b"(faddr),[smiport]"d"(smiport));
#elif __x86_64__
      __asm__ __volatile__( "push %%rbx;"
        "movq %[func], %%rax;"
        "movq %[faddr], %%rbx;"
        "movq %[smiport], %%rdx;"
        "out %%al, %%dx;"
        "movq %%rax, %[smiret];"
        "nop;"
        "nop;"
        "pop %%rbx;":[smiret]"+c"(smiret):[func]"a"(func),[faddr]"b"(faddr),[smiport]"d"(smiport));
#endif

      printk(KERN_INFO "after SMI_CALL_asm, func=%p, subfunc=%p, addr=%p, smiport=%x\n", func&0xff, func>>8, faddr, smiport);
      ret = smiret;
      printk("bytt, smi ret=%lx\n", ret);
      break;

    case IOCTL_UFU_RD_PHYSICAL_MEM:
      copy_from_user(&mem, (void *)arg, sizeof(UFU_MEM_OPERATION_BUFFER));
      addr = ioremap(mem.src, mem.size);
      printk(KERN_INFO "bytt, IOCTL_UFU_RD_PHYSICAL_MEM, src=%p, dest=%p, size=%u\n", mem.src, mem.dst, mem.size);
      ret = copy_to_user((void *)mem.dst, (void *)addr, mem.size);
      iounmap(addr);
      break;

    case IOCTL_UFU_RD_MSR64:
        printk(KERN_INFO "bytt, IOCTL_UFU_RD_MSR64, return false\n");
      break;

    default:
      ret = -EINVAL;
  }
  return ret;
}

static const struct file_operations ufu_fops =
{
  .owner = THIS_MODULE,
  .open = ufu_open,
  .release = ufu_release,
  .unlocked_ioctl = ufu_ioctl,
};

static int UFUDev_init(void)
{
  int result;

  //Dynamic allocation of device number
  result = alloc_chrdev_region(&devno,0,1,"ufudev");
  if(result < 0){
    printk("alloc_chrdev_region fail!\n");
    unregister_chrdev_region(devno,1);
    return result;
  }else{
    printk("alloc_chrdev_region sucess!\n");
  }

  cdev_init(&ufu_dev, &ufu_fops);
  ufu_dev.owner = THIS_MODULE;
  ufu_dev.ops = &ufu_fops;

  result = cdev_add(&ufu_dev, devno, UFUDEV_NR_DEVS);
  if (result) {
    printk(KERN_INFO "Error %d adding ufudev driver", result);
    goto Fail;
  }

  //Create class
  cdev_class = class_create(THIS_MODULE,"ufudev");
  if(IS_ERR(cdev_class)){
    printk("Create class fail!\n");
    unregister_chrdev_region(devno,1);
    return -1;
  }else{
    printk("Create class sucess!\n");
  }

  //Create device node ("mknod")
  device_create(cdev_class,NULL,devno,0,"ufudev");

  return 0;

Fail:
  unregister_chrdev_region(devno, 1);
  return result;
}

static void UFUDev_exit(void)
{
  device_destroy(cdev_class,devno);
  class_destroy(cdev_class);
  cdev_del(&ufu_dev);
  unregister_chrdev_region(devno,1);
  printk("UFUDev_exit sucess!\n");
}


module_init(UFUDev_init);
module_exit(UFUDev_exit);
MODULE_AUTHOR("Byosoft");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Flash update driver");
MODULE_ALIAS("Driver for Byosoft linux flash update utility");
