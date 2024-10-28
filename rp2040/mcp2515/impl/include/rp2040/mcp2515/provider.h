#ifndef RP2040_MCP2515_H_
#define RP2040_MCP2515_H_

#include <boost/preprocessor/repetition/repeat_from_to.hpp>

// core
#include "core/message.h"
#include "core/providers/base.h"

#include "mcp2515/mcp2515.h"

#define MCP2515_MAX_RAW_ISR 2

#define MCP2515_ISR_FUNC_HEADER(z, n, data) static void global_isr_##n(void);

#define MCP2515_ISR_FUNC_IMPL(z, n, data)           \
    void CANBus::global_isr_##n()                   \
    {                                               \
        return CANBus::rawIsrs[n]->rawIrqHandler(); \
    };

#define MCP2515_ISR_SWITCH(z, n, data) \
    case (n):                          \
        func = CANBus::global_isr_##n; \
        break;

namespace can::providers::rp2040::mcp2515
{

    // implementation specific
    enum class Bitrate : can::providers::base::bitrate_enum_t
    {
        BITRATE_5000 = CAN_SPEED::CAN_5KBPS,
        BITRATE_10000 = CAN_SPEED::CAN_10KBPS,
        BITRATE_20000 = CAN_SPEED::CAN_20KBPS,
        BITRATE_31250 = CAN_SPEED::CAN_31K25BPS,
        BITRATE_33000 = CAN_SPEED::CAN_33KBPS,
        BITRATE_40000 = CAN_SPEED::CAN_40KBPS,
        BITRATE_50000 = CAN_SPEED::CAN_50KBPS,
        BITRATE_80000 = CAN_SPEED::CAN_80KBPS,
        BITRATE_83300 = CAN_SPEED::CAN_83K3BPS,
        BITRATE_95000 = CAN_SPEED::CAN_95KBPS,
        BITRATE_100000 = CAN_SPEED::CAN_100KBPS,
        BITRATE_125000 = CAN_SPEED::CAN_125KBPS,
        BITRATE_200000 = CAN_SPEED::CAN_200KBPS,
        BITRATE_250000 = CAN_SPEED::CAN_250KBPS,
        BITRATE_500000 = CAN_SPEED::CAN_500KBPS,
        BITRATE_1000000 = CAN_SPEED::CAN_1000KBPS
    };

    enum class Clock : can::providers::base::clock_enum_t
    {
        CLOCK_8_MHZ = CAN_CLOCK::MCP_8MHZ,
        CLOCK_16_MHZ = CAN_CLOCK::MCP_16MHZ,
        CLOCK_20_MHZ = CAN_CLOCK::MCP_20MHZ
    };

    struct Options : can::providers::base::options_t
    {
        Clock clock;
        uint interrupt_pin;
    };

    class CANBus : can::providers::base::CANBus
    {
    private:
        MCP2515 chip;
        Options op;

        uint8_t bindToNextIsrId();
        void rawIrqHandler();

        // gpio_irq_level level;

    public:
        CANBus(can::providers::base::bitrate_enum_t b, can::providers::base::options_t o);
        virtual ~CANBus(); // desructor

        can::frame_read_res readMessage();

        static CANBus *rawIsrs[MCP2515_MAX_RAW_ISR];
        BOOST_PP_REPEAT(MCP2515_MAX_RAW_ISR, MCP2515_ISR_FUNC_HEADER, _)

        // Can only set one GPIO IRQ handler, so for most purposes implement this yourself.
        // see gpio_set_irq_callback(callback);

        /**
         * Normally when using `gpio_set_irq_enabled_with_callback()"`
         * (which calls `gpio_set_irq_callback()`) you can only have a
         * single GPIO IRQ callback per core (the default mechanism).
         *
         * However, we can use `gpio_add_raw_irq_handler()` to add
         * more explicit GPIO-specifc IRQ handlers that are **independent**
         * of the default callback mechanism.
         *
         * When `gpio_add_raw_irq_handler()` is called, the default callback
         * will no longer be called for the specified GPIO.
         *
         * See also: [`gpio_add_raw_irq_handler` documentation](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#group_hardware_gpio_1ga2e78fcd487a3a2e173322c6502fe9419).
         *
         * Note that there is only one interrupt request handler / IRQ for the whole GPIO
         * bank so internally  `gpio_add_raw_irq_handler()` chains registered callbacks which
         * are all called on any interrupt [source (forum user)](https://forums.raspberrypi.com/viewtopic.php?t=339696).
         *
         * Therefore each raw irq handler must mask IRQ events that are not used by the
         * handler as it will be called for each IRQ event.
         *
         */
        void registerRawIrqHandlerOnCurrentCore();
    };
}

#endif /* RP2040_MCP2515_H_ */