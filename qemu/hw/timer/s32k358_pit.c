#include "qemu/osdep.h"
#include "hw/timer/s32k358_pit.h"
#include "hw/irq.h"
#include "qemu/log.h"
#include "qemu/module.h"

/* Debug Printing */
#ifndef PIT_ERR_DEBUG
#define PIT_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (PIT_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

/* Timer interrupt handling */
static void s32k358_pit_timer_interrupt(void *opaque) {
    S32K358PITState *s = opaque;

    if (s->tctrl & PIT_TCTRL_TEN) {
        s->tflg |= 1; /* Set the Timer Interrupt Flag */
        if (s->tctrl & PIT_TCTRL_TIE) {
            qemu_irq_pulse(s->irq); /* Trigger the interrupt if enabled */
        }
    }
}

/* Timer configuration for alarm */
static void s32k358_pit_set_alarm(S32K358PITState *s) {
    if (s->tctrl & PIT_TCTRL_TEN) {
        int64_t interval_ns = muldiv64(s->ldval + 1, 1000000000, QEMU_CLOCK_VIRTUAL_RT);
        timer_mod(s->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + interval_ns);
    }
}

/* PIT reset function */
static void s32k358_pit_reset(DeviceState *dev) {
    S32K358PITState *s = S32K358_PIT(dev);

    s->mcr = 0x6; /* Default: Disable PIT (per manual) */
    s->ldval = 0xFFFFFFFF; /* Maximum load value */
    s->cval = 0xFFFFFFFF; /* Default current value */
    s->tctrl = 0; /* Disable the timer */
    s->tflg = 0; /* Clear interrupt flag */
}

/* Read function for PIT registers */
static uint64_t s32k358_pit_read(void *opaque, hwaddr offset, unsigned size) {
    S32K358PITState *s = opaque;

    switch (offset) {
        case PIT_MCR:
            return s->mcr;
        case PIT_LDVAL0:
            return s->ldval;
        case PIT_CVAL0:
            return s->cval; /* Simulate current value */
        case PIT_TCTRL0:
            return s->tctrl;
        case PIT_TFLG0:
            return s->tflg;
        default:
            qemu_log_mask(LOG_GUEST_ERROR, "Invalid read offset 0x%" HWADDR_PRIx "\n", offset);
            return 0;
    }
}

/* Write function for PIT registers */
static void s32k358_pit_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
    S32K358PITState *s = opaque;

    switch (offset) {
        case PIT_MCR:
            s->mcr = value & 0xFFFFFFFD; /* MDIS bit */
            break;
        case PIT_LDVAL0:
            s->ldval = value;
            break;
        case PIT_TCTRL0:
            s->tctrl = value & 0x7; /* Mask valid bits */
            if (s->tctrl & PIT_TCTRL_TEN) {
                s32k358_pit_set_alarm(s); /* Configure the alarm */
            } else {
                timer_del(s->timer); /* Stop the timer */
            }
            break;
        case PIT_TFLG0:
            if (value & 1) {
                s->tflg = 0; /* Clear the Timer Interrupt Flag */
            }
            break;
        default:
            qemu_log_mask(LOG_GUEST_ERROR, "Invalid write offset 0x%" HWADDR_PRIx "\n", offset);
            break;
    }
}

/* Memory region operations */
static const MemoryRegionOps s32k358_pit_ops = {
    .read = s32k358_pit_read,
    .write = s32k358_pit_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/* Initialization */
static void s32k358_pit_init(Object *obj) {
    S32K358PITState *s = S32K358_PIT(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);
    memory_region_init_io(&s->iomem, obj, &s32k358_pit_ops, s, "s32k358-pit", 0x200);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, s32k358_pit_timer_interrupt, s);
}

/* Realize */
static void s32k358_pit_realize(DeviceState *dev, Error **errp) {
    S32K358PITState *s = S32K358_PIT(dev);
    
    s->mcr = 0x6; /* Default: Disable PIT (per manual) */
    s->ldval = 0xFFFFFFFF; /* Maximum load value */
    s->cval = 0xFFFFFFFF; /* Default current value */
    s->tctrl = 0; /* Disable the timer */
    s->tflg = 0; /* Clear interrupt flag */
}

/* Class Initialization */
static void s32k358_pit_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, s32k358_pit_reset);
    dc->realize = s32k358_pit_realize;
}

/* Type Registration */
static const TypeInfo s32k358_pit_info = {
    .name = TYPE_S32K358_PIT,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(S32K358PITState),
    .instance_init = s32k358_pit_init,
    .class_init = s32k358_pit_class_init,
};

static void s32k358_pit_register_types(void) {
    type_register_static(&s32k358_pit_info);
}

type_init(s32k358_pit_register_types);

