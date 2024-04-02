#include "sgtl5000.h"

int sgtl_5000init(){
	uint16_t reg; // configuation des registres
	int check;
	sgtl_5000_i2c_read_register(SGTL5000_ADDR_CHIP_ID, &reg);
	printf("0x%04x\r\n", reg);
	check = sgtl_5000_i2c_clear_bit(SGTL5000_ADDR_CHIP_ANA_POWER, (1<<12) | ((1<<13)));
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_set_bit(SGTL5000_ADDR_CHIP_ANA_POWER, (1<<5) | ((1<<6)));
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_REF_CTRL, 0x01FF);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_LINE_OUT_CTRL, 0x031E);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_SHORT_CTRL, 0x1106);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_ANA_CTRL, 0x0004);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_ANA_POWER, 0x6AFF);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_DIG_POWER , 0x0073);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_LINE_OUT_VOL, 0x1111);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_CLK_CTRL, 0x0004);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_I2S_CTRL, 0x0130);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_ADCDAC_CTRL , 0x0000);
	if(check==-1)
		return check;
	check = sgtl_5000_i2c_write_register(SGTL5000_ADDR_CHIP_DAC_VOL ,  0x3C3C);
	if(check==-1)
		return check;
	return 0;

}

int sgtl_5000_i2c_read_register(uint16_t reg_adress, uint16_t * pdata){

	HAL_StatusTypeDef ret;
	uint8_t reg[2];
	ret = HAL_I2C_Mem_Read(&hi2c2, 0x14, reg_adress, I2C_MEMADD_SIZE_16BIT, reg, 2, HAL_MAX_DELAY);
	*pdata = (reg[0] << 8) + reg[1];

	if (ret==HAL_OK){
		return 0;

	}
	else{
		return -1;
	}

}


int sgtl_5000_i2c_write_register(uint16_t reg_adress, uint16_t pdata){

	HAL_StatusTypeDef ret;
	uint8_t reg[2];

	reg[0] = (pdata >>8);
	reg[1] = pdata & 0xFF;

	ret = HAL_I2C_Mem_Write(&hi2c2, 0x14, reg_adress, I2C_MEMADD_SIZE_16BIT, reg, 2, HAL_MAX_DELAY);

	if (ret==HAL_OK){
		return 0;

	}
	else{
		return -1;
	}

}

int sgtl_5000_i2c_set_bit(uint16_t reg_adress, uint16_t mask){
	uint16_t reg;
	sgtl_5000_i2c_read_register(reg_adress, &reg);
	reg = reg | mask;
	sgtl_5000_i2c_write_register(reg_adress, reg);
	return 0;


}

int sgtl_5000_i2c_clear_bit(uint16_t reg_adress, uint16_t mask){
	uint16_t reg;
	sgtl_5000_i2c_read_register(reg_adress, &reg);
	reg = reg & (~mask);
	sgtl_5000_i2c_write_register(reg_adress, reg);
	return 0;

}

