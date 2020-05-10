#ifndef HAL_QEMU_INTERRUPT_H_
#define HAL_QEMU_INTERRUPT_H_ 

typedef union CpuControl_t
{
    uint32_t all;
    struct {
        uint32_t Enable:1;          // 0
        uint32_t reserved:31;
    } bits;
} CpuControl_t;

typedef union PriorityMask_t
{
    uint32_t all;
    struct {
        uint32_t Prioritymask:8;    // 7:0
        uint32_t reserved:24;
    } bits;
} PriorityMask_t;

typedef union BinaryPoint_t
{
    uint32_t all;
    struct {
        uint32_t Binarypoint:3;     // 2:0
        uint32_t reserved:29;
    } bits;
} BinaryPoint_t;

typedef union InterruptAck_t
{
    uint32_t all;
    struct {
        uint32_t InterruptID:10;    // 9:0
        uint32_t CPUsourceID:3;     // 12:10
        uint32_t reserved:19;
    } bits;
} InterruptAck_t;

typedef union EndOfInterrupt_t
{
    uint32_t all;
    struct {
        uint32_t InterruptID:10;    // 9:0
        uint32_t CPUsourceID:3;     // 12:10
        uint32_t reserved:19;
    } bits;
} EndOfInterrupt_t;

typedef union RunningInterrupt_t
{
    uint32_t all;
    struct {
        uint32_t Reserved:4;        // 3:0
        uint32_t Priority:4;        // 7:4
        uint32_t reserved:24;
    } bits;
} RunningInterrupt_t;

typedef union HighestPendInter_t
{
    uint32_t all;
    struct {
        uint32_t InterruptID:10;    // 9:0
        uint32_t CPUsourceID:3;     // 12:10
        uint32_t reserved:19;
    } bits;
} HighestPendInter_t;

typedef union DistributorCtrl_t
{
    uint32_t all;
    struct {
        uint32_t Enable:1;          // 0
        uint32_t reserved:31;
    } bits;
} DistributorCtrl_t;

typedef union ControllerType_t
{
    uint32_t all;
    struct {
        uint32_t IDlinesnumber:5;   // 4:0
        uint32_t CPUnumber:3;       // 7:5
        uint32_t reserved:24;
    } bits;
} ControllerType_t;

typedef struct GicCput_t
{
    CpuControl_t       cpucontrol;        //0x000
    PriorityMask_t     prioritymask;      //0x004
    BinaryPoint_t      binarypoint;       //0x008
    InterruptAck_t     interruptack;      //0x00C
    EndOfInterrupt_t   endofinterrupt;    //0x010
    RunningInterrupt_t runninginterrupt;  //0x014
    HighestPendInter_t highestpendinter;  //0x018
} GicCput_t;

typedef struct GicDist_t
{
    DistributorCtrl_t   distributorctrl;    //0x000
    ControllerType_t    controllertype;     //0x004
    uint32_t            reserved0[62];      //0x008-0x0FC
    uint32_t            setenable[31];      //0x100-0x17C
    uint32_t            clearenable[31];    //0x180-0x1FC
} GicDist_t;

#define GIC_CPU_BASE  0x2C000000  //CPU interface
#define GIC_DIST_BASE 0x2F000000  //distributor 

#define GIC_PRIORITY_MASK_NONE  0xFF

/* 8.13.7 GICC_CTLR, CPU Interface Control Register */
#define GIC_CPU_CTLR_ENABLE			(0x1)	/* Enable GICC */
#define GIC_CPU_CTLR_DISABLE			(0x0)	/* Disable GICC */

/* 8.13.14 GICC_PMR, CPU Interface Priority Mask Register */
#define GIC_CPU_PMR_PRIO_MIN			(0xff)	/* The lowest level mask */
#define GIC_CPU_PMR_PRIO_HIGH			(0x0)	/* The highest level mask */

/* 8.13.11 GICC_IAR, CPU Interface Interrupt Acknowledge Register */
#define GIC_CPU_IAR_INTR_IDMASK		(0x3ff)	/* 0-9 bits means Interrupt ID */
#define GIC_CPU_IAR_SPURIOUS_INTR		(0x3ff)	/* 1023 means spurious interrupt */

/* 8.9.4 GICD_CTLR, Distributor Control Register */
#define GIC_DIST_CTLR_ENABLE	(0x1)	/* Enable GICD */
#define GIC_DIST_CTLR_DISABLE	(0x0)	/* Disable GICD */

#define GIC_DIST_ICENABLER_PER_REG		(32)
#define GIC_DIST_ISENABLER_PER_REG		(32)

#endif  /* HAL_QEMU_INTERRUPT_H_ */
