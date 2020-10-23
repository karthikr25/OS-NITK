#ifndef __MYOS__HARDWARECOMMUNICATION__PORT_H
#define __MYOS__HARDWARECOMMUNICATION__PORT_H

#include "types.h"
        
    class Port
    {
        protected:
            uint16_t portnumber;
            Port(uint16_t portnumber);
            // FIXME: Must be virtual (currently isnt because the kernel has no memory management yet)
            ~Port();
    };

    
    class Port8Bit : public Port
    {
        public:
            Port8Bit(uint16_t portnumber);
            ~Port8Bit();

            virtual uint8_t Read();
            virtual void Write(uint8_t data);
    };



    class Port8BitSlow : public Port8Bit
    {
        public:
            Port8BitSlow(uint16_t portnumber);
            ~Port8BitSlow();

            virtual void Write(uint8_t data);
    };



    class Port16Bit : public Port
    {
        public:
            Port16Bit(uint16_t portnumber);
            ~Port16Bit();

            virtual uint16_t Read();
            virtual void Write(uint16_t data);
    };

    class Port32Bit : public Port
    {
        public:
            Port32Bit(uint16_t portnumber);
            ~Port32Bit();

            virtual uint32_t Read();
            virtual void Write(uint32_t data);
    };

#endif