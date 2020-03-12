#include <stdint.h>
#include <stm8s.h>
#include <delay.h>
#include <utils.h>


// ROM 8 bytes
// 0 - device family
// 7 - crc
//const uint8_t ROM[] = {0x28, 0xF2, 0x27, 0xD6, 0x04, 0x00, 0x00, 0x3D};
const uint8_t ROM[] = {0x28, 0x86, 0x51, 0xDD, 0x06, 0x00, 0x00, 0x53};  // my DS18B20 sensor


// 0 - low byte temperature
// 1 - high byte temperature
// 2..6 -  reserved
// 7 - crc
uint8_t SCRATCHPAD[] = {0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


// create 8 bit crc from data
uint8_t crc8(const uint8_t* data, short num_bytes)
{
  uint8_t crc = 0;

  while (num_bytes--)
  {
    uint8_t inbyte = *data++;
    for (uint8_t i = 8; i; i--)
    {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix)
        crc ^= 0x8C;
      inbyte <<= 1;
    }
  }

  return crc;
}

// TODO
#define OW_PRESENSE_DURATION 60
#define OW_PIN 4
#define LED_PIN 5


// TODO write mapping to string
typedef enum {
  OW_SUCCESS  = (uint8_t) 0,        // success
  OW_ERROR_SEND_BIT  = (uint8_t) 1, // failed to send bit
  OW_ERROR_RECV_BIT  = (uint8_t) 2, // failed to receive bit
}ow_error_t;


void init_ow_pin()
{
	// select Open drain output, fast mode
  set_bit(PD_DDR, OW_PIN);
  clr_bit(PD_CR1, OW_PIN);  // open drain
  set_bit(PD_CR2, OW_PIN);  // fast mode
}


void pull_low()
{
  clr_bit(PD_ODR, OW_PIN);
}


void release_bus()
{
  set_bit(PD_ODR, OW_PIN);
}


// we wait till master pulls bus low (communication is always initiated by bus master)
void wait_till_pull_low(void)
{
  while (get_bit(PD_IDR, OW_PIN));
  //while (PD_IDR & 0b00001000);
}


void wait_till_bus_release(void)
{
  while (!get_bit(PD_IDR, OW_PIN));
}


// send zero bit
uint8_t send_bit_0(void)
{
  wait_till_pull_low();
  pull_low();
  delay_us(60);
  release_bus();
  return OW_SUCCESS;
}


// send one bit
uint8_t send_bit_1(void)
{
  wait_till_pull_low();
  delay_us(60);
  return OW_SUCCESS;
}


// send single bit
uint8_t send_bit(uint8_t bit)
{
  if (bit)
    return send_bit_1();
  else
    return send_bit_0();
}


// receive single bit
uint8_t recv_bit(uint8_t* bit)
{
  wait_till_pull_low();
  delay_us(30);
  *bit = get_bit(PD_IDR, OW_PIN);
  delay_us(30);  
  return OW_SUCCESS;
}


void wait_for_reset()
{
  while(1)
  {
    wait_till_pull_low();
    reset_tim4_counter();
    wait_till_bus_release();
    if (read_tim4_counter() / 5 >= 480)  // reset duration >= 480us
      break;
  }
}


void wait_for_reset1()
{
  uint8_t cnt = 0;
  while(1)
  {
    if (!get_bit(PD_IDR, OW_PIN))
      if (cnt < 8)
        cnt++;
    else
    {
      if (cnt == 8)
        break;
      else
        cnt = 0;
        continue;
    }
    delay_us(40);
  }
}

// indicate devices presence on the bus
void indicate_presence()
{
  //send_bit_0();
  //wait_till_pull_low();
  wait_till_bus_release();
  delay_us(20);
  pull_low();
  delay_us(60);
  release_bus();
}


// listen to the bus until a command is received
// TODO detect reset
uint8_t wait_for_cmd(uint8_t* cmd)
{
  uint8_t recv_byte = 0;
  uint8_t bit_pos = 0;
  uint8_t bit;
  uint8_t error;

  while (bit_pos < 8)
  {
    error = recv_bit(&bit);
    if (error)
    {
      // ERROR("Failed to read bit");
      return OW_ERROR_RECV_BIT;
    }
    //recv_byte |= (bit << bit_pos);
    if (bit)
      set_bit(recv_byte, bit_pos);
    else
      clr_bit(recv_byte, bit_pos);

    ++bit_pos;
  }
  *cmd = recv_byte;
  return OW_SUCCESS;
}


// participate in search
void search_rom()
{
  uint8_t byte_pos = 0;
  uint8_t bit_pos = 0;
  uint8_t error;
  uint8_t bit;
  uint8_t direction_bit;

  while (1)  // bits in ROM
  {
    bit = get_bit(ROM[byte_pos], bit_pos);

    // send true bit
    error = send_bit(bit);
    if (error)
    {
      // ERROR("Failed to send bit");
      return;
    }

    // send complement bit
    tgl_bit(bit, 0);
    error = send_bit(bit);
    if (error)
    {
      // ERROR("Failed to send bit");
      return;
    }

    error = recv_bit(&direction_bit);
    if (error)
    {
      // ERROR("Failed to read bit");
      return;
    }
    if (direction_bit != bit)
      break;

    ++bit_pos;
    if (bit_pos == 8)
    {
      bit_pos = 0;
      ++byte_pos;
    }
    if (byte_pos == 8)
      break;  // TODO we are the last device!
  }
}


// send data
uint8_t send_data(uint8_t* data, uint8_t length)
{
  uint8_t byte_pos = 0;
  uint8_t bit_pos = 0;
  uint8_t error;
  uint8_t bit;
  uint8_t direction_bit;

  while (1)  // bits in ROM
  {
    bit = get_bit(data[byte_pos], bit_pos);

    // send true bit
    error = send_bit(bit);
    if (error)
    {
      // ERROR("Failed to send bit");
      return;
    }

    ++bit_pos;
    if (bit_pos == 8)
    {
      bit_pos = 0;
      ++byte_pos;
    }
    if (byte_pos == length)
      break;
  }
}


// ow_slave
void main() {
  uint8_t cmd;
  uint8_t error;
  init_ow_pin();
  release_bus();

  set_bit(PB_DDR, LED_PIN);
  clr_bit(PB_CR1, LED_PIN);  // open drain
  set_bit(PB_ODR, LED_PIN);  // switch off

  initialize_system_clock();
  enable_interrupts();

  while (1)
  {
    /*
    pull_low();
    delay_us(20);
    release_bus();
    delay_us(20);
    */

    wait_till_pull_low();
    wait_for_reset1();
    //wait_for_reset();
    indicate_presence();

    while (1)
    {
      error = wait_for_cmd(&cmd);
      if (error)
        break;

      if (cmd == 0xF0)
      {
        // search ROM
        search_rom();
      }
      else if (cmd == 0x33)
      {
        // read ROM
        send_data(ROM, 8);
        //clr_bit(PB_ODR, LED_PIN);
      }
      else if (cmd == 0xB4)
      {
        // parasidic power
        send_bit(0);
        break;  // this is followed by a reset
      }
      else if (cmd == 0xCC)
      {
        // skip ROM
        // we don't have to do anything
      }
      else if (cmd == 0xBE)
      {
        // read scratchpad
        SCRATCHPAD[8] = crc8(SCRATCHPAD, 8);
        send_data(ROM, 8);
        //break;  // this is followed by a reset
      }
      else
      {
        // unknown command
        break;
      }
      
    }

  }

}
