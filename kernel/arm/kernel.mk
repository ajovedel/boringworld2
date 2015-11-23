ARM_OBJS := swi_handler.o irq_handler.o c_swi_handler.o c_irq_handler.o interrupt_setup.o timer_setup.o install_handler.o reg.o psr.o 
ARM_OBJS := $(ARM_OBJS:%=$(KDIR)/arm/%)

KOBJS += $(ARM_OBJS)
