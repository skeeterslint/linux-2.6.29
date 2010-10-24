/*
 * wm8994_aries.c  --  WM8994 ALSA Soc Audio driver related Aries
 *
 *  Copyright (C) 2010 Samsung Electronics.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/gpio.h> 
#include <plat/gpio-cfg.h> 
#include <plat/map-base.h>
#include <plat/regs-clock.h>
#include <mach/gpio-jupiter.h>
#include "wm8994.h"

//------------------------------------------------
//		Debug Feature
//------------------------------------------------
#define SUBJECT "wm8994_aries.c"
#define AUDIO_CODEC_WM8994_DEBUG  1


//------------------------------------------------
// Definitions of tunning volumes for wm8994
//------------------------------------------------

//------------------------------------------------
// Common

// DAC
#define TUNING_DAC1L_VOL		0xC0		// 610h

#define TUNING_DAC1R_VOL		0xC0		// 611h
#define TUNING_DAC2L_VOL		0xC0		// 612h
#define TUNING_DAC2R_VOL		0xC0		// 613h

// Speaker
#define TUNING_SPKMIXL_ATTEN		0x0		// 22h
#define TUNING_SPKMIXR_ATTEN		0x0		// 23h

// Headset
#define TUNING_EAR_OUTMIX5_VOL		0x0		// 31h
#define TUNING_EAR_OUTMIX6_VOL		0x0		// 32h

//------------------------------------------------
// Normal
// Speaker
#define TUNING_MP3_SPKL_VOL		0x3F		// 26h
#define TUNING_MP3_CLASSD_VOL		0x6		// 25h

// Headset
#define TUNING_MP3_OUTPUTL_VOL		0x34		// 1Ch
#define TUNING_MP3_OUTPUTR_VOL		0x34		// 1Dh
#define TUNING_MP3_OPGAL_VOL		0x39		// 20h
#define TUNING_MP3_OPGAR_VOL		0x39		// 21h

// Dual
#define TUNING_MP3_DUAL_OUTPUTL_VOL		0x1E		// 1Ch
#define TUNING_MP3_DUAL_OUTPUTR_VOL		0x1E		// 1Dh

// Extra_Dock_speaker
#define TUNING_MP3_EXTRA_DOCK_SPK_OPGAL_VOL		0x39		// 20h
#define TUNING_MP3_EXTRA_DOCK_SPK_OPGAR_VOL		0x39		// 21h
#define TUNING_EXTRA_DOCK_SPK_OUTMIX5_VOL		0x0		// 31h
#define TUNING_EXTRA_DOCK_SPK_OUTMIX6_VOL		0x0		// 32h
#define TUNING_MP3_EXTRA_DOCK_SPK_VOL	0x0		//1Eh

//------------------------------------------------
// Ringtone
// Speaker
#define TUNING_RING_SPKL_VOL		0x3E		// 26h
#define TUNING_RING_CLASSD_VOL		0x5		// 25h

// Headset
#define TUNING_RING_OUTPUTL_VOL		0x34		// 1Ch
#define TUNING_RING_OUTPUTR_VOL		0x34		// 1Dh
#define TUNING_RING_OPGAL_VOL		0x39		// 20h
#define TUNING_RING_OPGAR_VOL		0x39		// 21h

// Dual
#define TUNING_RING_DUAL_OUTPUTL_VOL		0x1E		// 1Ch
#define TUNING_RING_DUAL_OUTPUTR_VOL		0x1E		// 1Dh

//------------------------------------------------
// Call
// Speaker
#define TUNING_CALL_SPKL_VOL		0x3E		// 26h
#define TUNING_CALL_CLASSD_VOL		0x7		// 25h

// Headset
#define TUNING_CALL_OUTPUTL_VOL		0x30		// 1Ch
#define TUNING_CALL_OUTPUTR_VOL		0x30		// 1Dh
#define TUNING_CALL_OPGAL_VOL		0x39		// 20h
#define TUNING_CALL_OPGAR_VOL		0x39		// 21h

// Receiver
#define TUNING_RCV_OUTMIX5_VOL		0x0		// 31h
#define TUNING_RCV_OUTMIX6_VOL 		0x0		// 32h
#define TUNING_RCV_OPGAL_VOL		0x3D		// 20h
#define TUNING_RCV_OPGAR_VOL		0x3D		// 21h
#define TUNING_HPOUT2_VOL		0x0		// 1Fh

// Call Main MIC
#define TUNING_CALL_RCV_INPUTMIX_VOL	0x16		// 18h
#define TUNING_CALL_RCV_MIXER_VOL	WM8994_IN1L_MIXINL_VOL	// 29h 30dB

#define TUNING_CALL_SPK_INPUTMIX_VOL	0x0D		// 18h
#define TUNING_CALL_SPK_MIXER_VOL	WM8994_IN1L_MIXINL_VOL		// 29h 0dB

// Call Ear MIC
#define TUNING_CALL_EAR_INPUTMIX_VOL	0x1D		// 1Ah

//------------------------------------------------
// FM Radio
// Speaker
#define TUNING_FMRADIO_SPKL_VOL		0x3E		// 26h
#define TUNING_FMRADIO_CLASSD_VOL	0x6		// 25h

// Headset
#define TUNING_FMRADIO_OUTPUTL_VOL	0x3C		// 1Ch
#define TUNING_FMRADIO_OUTPUTR_VOL	0x3C		// 1Dh
#define TUNING_FMRADIO_OPGAL_VOL	0x39		// 20h
#define TUNING_FMRADIO_OPGAR_VOL	0x39		// 21h

//Input
#define TUNING_DAC1L_RADIO_VOL		0xA8		// 402h
#define TUNING_DAC1R_RADIO_VOL		0xA8		// 403h

// FM Radio Input
#define TUNING_FMRADIO_EAR_INPUTMIXL_VOL	0x0B		// 19h
#define TUNING_FMRADIO_EAR_INPUTMIXR_VOL	0x0B		// 1Bh

#define TUNING_FMRADIO_SPK_INPUTMIXL_VOL	0x0F		// 19h
#define TUNING_FMRADIO_SPK_INPUTMIXR_VOL	0x0F		// 1Bh

//------------------------------------------------
// Recording
// Main MIC
#define TUNING_RECORD_MAIN_INPUTLINE_VOL	0x18		// 18h
#define TUNING_RECORD_MAIN_AIF1ADCL_VOL	0xFF		// 400h
#define TUNING_RECORD_MAIN_AIF1ADCR_VOL	0xFF		// 401h

#define TUNING_RECOGNITION_MAIN_INPUTLINE_VOL	0x1C		// 18h
#define TUNING_RECOGNITION_MAIN_AIF1ADCL_VOL	0xD0		// 400h
#define TUNING_RECOGNITION_MAIN_AIF1ADCR_VOL	0xD0		// 401h

// Ear MIC
#define TUNING_RECORD_SUB_INPUTMIX_VOL	0x15		// 1Ah
#define TUNING_RECORD_SUB_AIF1ADCL_VOL	0xC0		// 400h
#define TUNING_RECORD_SUB_AIF1ADCR_VOL	0xC0		// 401h

#define TUNING_RECOGNITION_SUB_INPUTMIX_VOL	0x10		// 1Ah
#define TUNING_RECOGNITION_SUB_AIF1ADCL_VOL	0xC0		// 400h
#define TUNING_RECOGNITION_SUB_AIF1ADCR_VOL	0xC0		// 401
//------------------------------------------------


//------------------------------------------------
// Definition external function prototype.
//------------------------------------------------
extern int hw_version_check(void);
extern short int get_headset_status(void);		// For ear-jack control(MIC-Bias)
extern void set_recording_status(int value);	// For preventing MIC Bias off on using MIC.
extern unsigned int tty_mode;
extern unsigned int loopback_mode;

int audio_init(void)
{

	//CODEC LDO SETTING
	if (gpio_is_valid(GPIO_CODEC_LDO_EN))
	{
		if (gpio_request(GPIO_CODEC_LDO_EN, "CODEC_LDO_EN"))
			DEBUG_LOG_ERR("Failed to request CODEC_LDO_EN! \n");
		gpio_direction_output(GPIO_CODEC_LDO_EN, 0);
	}

	s3c_gpio_setpull(GPIO_CODEC_LDO_EN, S3C_GPIO_PULL_NONE);
	
	// For preserving output of codec related pins.
	s3c_gpio_slp_cfgpin(GPIO_CODEC_LDO_EN, S3C_GPIO_SLP_PREV);

	//CODEC XTAL CLK SETTING
	//b4 : AP Gpio emul, B5 : CODEC_XTAL_EN 
#if (defined CONFIG_JUPITER_VER_B5) || (defined CONFIG_ARIES_VER_B0)
	if (gpio_is_valid(GPIO_CODEC_XTAL_EN)) {
		if (gpio_request(GPIO_CODEC_XTAL_EN, "GPIO_CODEC_XTAL_EN")) 
			DEBUG_LOG_ERR("Failed to request GPIO_CODEC_XTAL_EN! \n");
		
		gpio_direction_output(GPIO_CODEC_XTAL_EN, 0);
	}
	s3c_gpio_setpull(GPIO_CODEC_XTAL_EN, S3C_GPIO_PULL_NONE);
#endif

	s3c_gpio_slp_setpull_updown(GPIO_CODEC_LDO_EN, S3C_GPIO_PULL_NONE);

	// PCM_SEL Setting
#if  (defined CONFIG_JUPITER_VER_B5) || (defined CONFIG_ARIES_VER_B0) || (defined CONFIG_ARIES_VER_B1)
	gpio_direction_output(GPIO_PCM_SEL, 0);
	s3c_gpio_setpull(GPIO_PCM_SEL, S3C_GPIO_PULL_NONE);
	gpio_set_value(GPIO_PCM_SEL, 0);
#endif

#if (defined CONFIG_JUPITER_VER_B5)  || (defined CONFIG_ARIES_VER_B0)
	s3c_gpio_slp_setpull_updown(GPIO_CODEC_XTAL_EN, S3C_GPIO_PULL_NONE);
#endif

	// FM radio reset pin control
	s3c_gpio_slp_cfgpin(GPIO_FM_RST, S3C_GPIO_SLP_PREV);
		
	return 0;

}

int audio_power(int en)
{
	u32 val;

#if (defined CONFIG_JUPITER_VER_B4)
	DEBUG_LOG("LDO Enable = %d, XTAL CLK From AP EMUL", en);
	val = __raw_readl(S5P_CLK_OUT);
	val &= 0xFFFE0FFF; //clear bits 12~16
#endif

	if(en)
	{
		// Forbid to turn off MCLK in sleep mode.
		val = __raw_readl(S5P_SLEEP_CFG);
		val |= (S5P_SLEEP_CFG_USBOSC_EN);
		__raw_writel(val , S5P_SLEEP_CFG);

		// Turn on LDO for codec.
		gpio_set_value(GPIO_CODEC_LDO_EN, 1);

		msleep(10);	// Wait for warming up.

		// Turn on master clock.
#if (defined CONFIG_JUPITER_VER_B5)  || (defined CONFIG_ARIES_VER_B0)
		DEBUG_LOG("LDO Enable = %d, XTAL CLK From OSI", en);
		gpio_set_value(GPIO_CODEC_XTAL_EN, 1);
#elif (defined CONFIG_ARIES_VER_B1) || (defined CONFIG_ARIES_VER_B2) || (defined CONFIG_ARIES_VER_B3)
#if (defined CONFIG_SND_UNIVERSAL_WM8994_MASTER)
		if(hw_version_check())
		{
			__raw_writel(__raw_readl(S5P_OTHERS) | (3<<8) , S5P_OTHERS); 
			__raw_writel(__raw_readl(S5P_CLK_OUT) | (0x1) , S5P_CLK_OUT);	
		}
#endif
#elif (defined CONFIG_JUPITER_VER_B4)
#ifdef CONFIG_SND_UNIVERSAL_WM8994_MASTER
		val |= (0x11 << 12); //crystall
#else
		val |= (0x02 << 12); // epll
		val |= (0x5 << 20);
#endif	// end of CONFIG_SND_UNIVERSAL_WM8994_MASTER
		 __raw_writel(val, S5P_CLK_OUT);
#endif	//end of CONFIG_JUPITER_VER_05
	}
	else
	{
		// Turn off LDO for codec.
		gpio_set_value(GPIO_CODEC_LDO_EN, 0);

		msleep(125);	// Wait to turn off codec entirely.

		// Turn on master clock.
#if (defined CONFIG_JUPITER_VER_B5) ||(defined CONFIG_ARIES_VER_B0)
		DEBUG_LOG("LDO Disable = %d, XTAL CLK From OSI", en);
		gpio_set_value(GPIO_CODEC_XTAL_EN, 0);
#elif (defined CONFIG_JUPITER_VER_B4)
		DEBUG_LOG("LDO Disable = %d, XTAL CLK From AP EMUL", en);
#ifdef CONFIG_SND_UNIVERSAL_WM8994_MASTER
		val &= ~(0x11 << 12); //crystall
#else
		val &= ~(0x02 << 12); // epll
		val &= ~(0x5 << 20);
#endif	//end of CONFIG_SND_UNIVERSAL_WM8994_MASTER
#elif (defined CONFIG_ARIES_VER_B1) || (defined CONFIG_ARIES_VER_B2) || (defined CONFIG_ARIES_VER_B3)
#if (defined CONFIG_SND_UNIVERSAL_WM8994_MASTER)
		__raw_writel(__raw_readl(S5P_OTHERS) & (~(0x3<<8)) , S5P_OTHERS);
		__raw_writel(__raw_readl(S5P_CLK_OUT) & (0xFFFFFFFE) , S5P_CLK_OUT);
#endif
#else
		__raw_writel(val, S5P_CLK_OUT);	
#endif	//end of CONFIG_JUPITER_VER_05

		// Allow to turn off MCLK in sleep mode.
		val = __raw_readl(S5P_SLEEP_CFG);
		val &= ~(S5P_SLEEP_CFG_USBOSC_EN);
		__raw_writel(val , S5P_SLEEP_CFG);
	}

	DEBUG_LOG("AUDIO POWER COMPLETED : %d", en);

	return 0;
}

void audio_ctrl_mic_bias_gpio(int enable)
{
	int headset_state;
	
	if(enable)
	{
		DEBUG_LOG("enable = [%d]", enable);
		set_recording_status(1);
		gpio_set_value(GPIO_MICBIAS_EN, 1);
	}
	else
	{
		set_recording_status(0);
		headset_state = get_headset_status();
		//ashton codec shutdown �� mic bias disable
		//if( headset_state == 0 ||headset_state == 0x1 << 5 )	// TV out or no headset
		//{
		//	DEBUG_LOG("enable = [%d]", enable);
			gpio_set_value(GPIO_MICBIAS_EN, 0);
		//}
	}
}

/*Audio Routing routines for the universal board..wm8994 codec*/
void wm8994_disable_playback_path(struct snd_soc_codec *codec, enum playback_path path)
{
	u16 val;
	struct wm8994_priv *wm8994 = codec->private_data;

	DEBUG_LOG("Path = [%d]", path);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);	
	
	switch(path)
	{
		case RCV:		
			//Disbale the HPOUT2
			val &= ~(WM8994_HPOUT2_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);
			
			// Disable left MIXOUT
			val = wm8994_read(codec,WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1,val);

			// Disable right MIXOUT
			val = wm8994_read(codec,WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_2,val);

			// Disable HPOUT Mixer
			val = wm8994_read(codec,WM8994_HPOUT2_MIXER);
			val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK | WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
			wm8994_write(codec,WM8994_HPOUT2_MIXER,val);

			// Disable mixout volume control
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_3,val);			
			break;

		case SPK:
			//Disbale the SPKOUTL
			val &= ~(WM8994_SPKOUTL_ENA_MASK); 
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// Disable SPKLVOL
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKLVOL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			// Disable SPKOUT mixer
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS);
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
			
			//Mute Speaker mixer
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER ,val);
			break;

		case HP:
			if(wm8994->codec_state & CALL_ACTIVE)
			{
			val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
			val &= ~(0x02C0);
			val |= 0x02C0;
			wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, 0x02C0);

			val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
			val &= ~(0x02C0);
			val |= 0x02C0;
			wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, 0x02C0);

			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
			val &= ~(0x0022);
			val |= 0x0022;
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, 0x0022);

			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(0x0);
			val |= 0x0;
			wm8994_write(codec, WM8994_OUTPUT_MIXER_1, 0x0);

			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(0x0);
			val |= 0x0;
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, 0x0);

			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
			val &= ~(0x0300);
			val |= 0x0300;
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x0300);

			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~(0x1F25);
			val |= 0x1F25;
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, 0x1F25);
			}
			break;

		case BT :
			//R6(6h) - 0x0000
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_6);
			val &= ~(WM8994_AIF3_ADCDAT_SRC_MASK | WM8994_AIF2_ADCDAT_SRC_MASK | WM8994_AIF2_DACDAT_SRC_MASK | WM8994_AIF1_DACDAT_SRC_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, val);

			//R1056(420h) - 0x0200
			val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
			val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
			val |= (WM8994_AIF1DAC1_MUTE);
			wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val); 
			break;

		case DUAL :
			if(wm8994->codec_state & CALL_ACTIVE)
			{
			val &= ~(WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK |WM8994_SPKOUTL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// ------------------ Ear path setting ------------------
			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
			
			//Disable Charge Pump	
			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~WM8994_CP_ENA_MASK ;
			val |=  WM8994_CP_ENA_DEFAULT ; // this is from wolfson	
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);
			
			// Intermediate HP settings
			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
			val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
				WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

			// ------------------ Spk path setting ------------------		
			// Disable SPKLVOL
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKLVOL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			// Disable SPKOUT mixer
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS);
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
			
			//Mute Speaker mixer
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER ,val);
			}
			break;	
		case EXTRA_DOCK_SPEAKER:
            /* Output Path Volume */
            wm8994_write(codec, 0x001E, 0x0006);    // Lineout Volume

            /* Output Path Routing */
			wm8994_write(codec, 0x002D, 0x0000);    // Output Mixer 1
			wm8994_write(codec, 0x002E, 0x0000);    // Output Mixer 2
			wm8994_write(codec, 0x0035, 0x0000);    // Line Mixer 2
			wm8994_write(codec, 0x0003, 0x0000);    // Power Management 3
			wm8994_write(codec, 0x0005, 0x0000);    // Power Management5
			break;

		default:
			DEBUG_LOG_ERR("Path[%d] is not invaild!\n", path);
			return;
			break;
	}
}

void wm8994_disable_rec_path(struct snd_soc_codec *codec, enum mic_path rec_path) 
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	wm8994->rec_path = MIC_OFF;
	audio_ctrl_mic_bias_gpio(0);	// Disable MIC bias
	
        switch(rec_path)
        {
                case MAIN:
			DEBUG_LOG_INFO("Disabling MAIN Mic Path..\n");
			
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2  );
			val &= ~( WM8994_IN1L_ENA_MASK | WM8994_MIXINL_ENA_MASK );
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);
			
			if(!wm8994->testmode_config_flag)
			{	
				// Mute IN1L PGA, update volume
				val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
				val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
				val |= (WM8994_IN1L_VU |WM8994_IN1L_MUTE); //volume
				wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
			
				//Mute the PGA
				val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
				val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
			}
			
			//Disconnect IN1LN ans IN1LP to the inputs
			val = wm8994_read(codec,WM8994_INPUT_MIXER_2 ); 
			val &= (WM8994_IN1LN_TO_IN1L_MASK | WM8994_IN1LP_TO_IN1L_MASK );
			wm8994_write(codec, WM8994_INPUT_MIXER_2, val);
			
			//Digital Paths
			
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4 );
			val &= ~(WM8994_ADCL_ENA_MASK |WM8994_AIF1ADC1L_ENA_MASK  );
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);

			//Disable timeslots
			val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING );
			val |=WM8994_ADC1L_TO_AIF1ADC1L ;  
			wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,val);			
			break;

                case SUB:
			DEBUG_LOG_INFO("Disbaling SUB Mic path..\n");
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2 );
			val &= ~(WM8994_IN1R_ENA_MASK |WM8994_MIXINR_ENA_MASK  );
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);
			
			if(!wm8994->testmode_config_flag)
			{	
				// Disable volume,unmute Right Line	
				val = wm8994_read(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);	
				val &= ~(WM8994_IN1R_MUTE_MASK | WM8994_IN1R_VOL_MASK);	// Unmute IN1R
				val |= (WM8994_IN1R_VU | WM8994_IN1R_MUTE);
				wm8994_write(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME,val);
				
				// Mute right pga, set volume 
				val = wm8994_read(codec,WM8994_INPUT_MIXER_4 );
				val&= ~(WM8994_IN1R_TO_MIXINR_MASK | WM8994_IN1R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
				wm8994_write(codec,WM8994_INPUT_MIXER_4 ,val);
			}
			
			//Disconnect in1rn to inr1 and in1rp to inrp
			val = wm8994_read(codec,WM8994_INPUT_MIXER_2);
			val &= ~( WM8994_IN1RN_TO_IN1R_MASK | WM8994_IN1RP_TO_IN1R_MASK);
			wm8994_write(codec,WM8994_INPUT_MIXER_2,val);
			
			//Digital Paths 
			//Disable right ADC and time slot
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
			val &= ~(WM8994_ADCR_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK );
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);
			
			//ADC Right mixer routing
			val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
			val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK);
			wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);
			break;
	
                case BT_REC:
			//R1542(606h) - 0x0000
			val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
			val &= ~(WM8994_AIF2DACL_TO_AIF1ADC1L_MASK | WM8994_ADC1L_TO_AIF1ADC1L_MASK);
			wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

			//R1543(607h) - 0x0000
			val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
			val &= ~(WM8994_AIF2DACR_TO_AIF1ADC1R_MASK | WM8994_ADC1R_TO_AIF1ADC1R_MASK);
			wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);

			//R1312(520h) - 0x0200
			val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);
			val &= ~(WM8994_AIF2DAC_MUTE_MASK);
			val |= (WM8994_AIF2DAC_MUTE);
			wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val); 
			break;

		default:
			DEBUG_LOG_ERR("Path[%d] is not invaild!\n", rec_path);
                	break;
        }
}

void wm8994_disable_fmradio_path(struct snd_soc_codec *codec, enum fmradio_path path)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("Turn off fmradio_path = [%d]", path);

	switch(path)
	{
		case FMR_OFF :
			wm8994->fmradio_path = FMR_OFF;

			//---------------------------------------------------
			// Disable speaker setting for FM radio
			if(wm8994->codec_state & CALL_ACTIVE)
			{

			//disbale the SPKOUTL
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_SPKOUTL_ENA_MASK); 
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_1 ,val);

			// Disable SPK Volume.
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(!wm8994->testmode_config_flag)
			{	
				// Mute the SPKMIXVOLUME
				val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
				val &= ~(WM8994_SPKMIXL_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
					
				val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
				val &= ~(WM8994_SPKMIXR_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
				val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
				val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);
			
				val = wm8994_read(codec, WM8994_CLASSD);
				val &= ~(WM8994_SPKOUTL_BOOST_MASK);
				wm8994_write(codec, WM8994_CLASSD, val);
			}
			
			/*Output MIxer-Output PGA*/
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
							
			// Output mixer setting
			val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_MIXINL_TO_SPKMIXL_MASK | WM8994_MIXINR_TO_SPKMIXR_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

			//---------------------------------------------------
			// Disable earpath setting for FM radio

			//Disable end point for preventing pop up noise.
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// Disable MIXOUT
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(!wm8994->testmode_config_flag)
			{
				// Output setting
				val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
				wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);
			
				val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
				wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
			}

			//Disable Charge Pump	
			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~WM8994_CP_ENA_MASK ;
			val |= WM8994_CP_ENA_DEFAULT ; // this is from wolfson		
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);
			
			// Intermediate HP settings
			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
			val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
				WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);
												
			// Disable Output mixer setting
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_MIXINL_TO_MIXOUTL_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
			
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_MIXINR_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val); 

			//---------------------------------------------------
			// Disable common setting for FM radio
			
			// Disable IN2 and MIXIN
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
			val &= ~(WM8994_TSHUT_ENA_MASK | WM8994_TSHUT_OPDIS_MASK | WM8994_OPCLK_ENA_MASK | 
					WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);
			
			// Disable Input mixer setting
			val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
			val &= ~(WM8994_IN2LN_TO_IN2L_MASK | WM8994_IN2RN_TO_IN2R_MASK);
			wm8994_write(codec, WM8994_INPUT_MIXER_2, val); 	
			
			if(!wm8994->testmode_config_flag)
			{
				// Disable IN2L to MIXINL
				val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
				val &= ~(WM8994_IN2L_TO_MIXINL_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
				
				//Disable IN2R to MIXINR
				val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
				val &= ~(WM8994_IN2R_TO_MIXINR_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_4, val); 	
			}
							
			// Mute IN2L PGA volume
			val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME);
			val &= ~(WM8994_IN2L_VU_MASK | WM8994_IN2L_MUTE_MASK | WM8994_IN2L_VOL_MASK);
			val |= (WM8994_IN2L_VU | WM8994_IN2L_MUTE);
			wm8994_write(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME, val); 
			
			val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);
			val &= ~(WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
			val |= (WM8994_IN2R_VU |WM8994_IN2R_MUTE);	
			wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val); 

			//---------------------------------------------------
			// Disable path setting for mixing
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);			

			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);				
			}
			break;
		
		case FMR_SPK :
			//disbale the SPKOUTL
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_SPKOUTL_ENA_MASK); 
//			wm8994_write(codec,WM8994_POWER_MANAGEMENT_1 ,val);

			// Disable SPK Volume.
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
//			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(!wm8994->testmode_config_flag)
			{	
				// Mute the SPKMIXVOLUME
				val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
				val &= ~(WM8994_SPKMIXL_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
					
				val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
				val &= ~(WM8994_SPKMIXR_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
				val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
				val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);
			
				val = wm8994_read(codec, WM8994_CLASSD);
				val &= ~(WM8994_SPKOUTL_BOOST_MASK);
				wm8994_write(codec, WM8994_CLASSD, val);
			}
			
			/*Output MIxer-Output PGA*/
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
							
			// Output mixer setting
			val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_MIXINL_TO_SPKMIXL_MASK | WM8994_MIXINR_TO_SPKMIXR_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);
			break;

		case FMR_HP :
			//Disable end point for preventing pop up noise.
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// Disable MIXOUT
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(!wm8994->testmode_config_flag)
			{
				// Output setting
				val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
				wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);
			
				val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
				wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
			}

			//Disable Charge Pump	
			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~WM8994_CP_ENA_MASK ;
			val |= WM8994_CP_ENA_DEFAULT ; // this is from wolfson		
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);
			
			// Intermediate HP settings
			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
			val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
				WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);
												
			// Disable Output mixer setting
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_MIXINL_TO_MIXOUTL_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
			
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_MIXINR_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val); 
			break;

		case FMR_SPK_MIX :
			//Mute the DAC path
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);			
			break;

		case FMR_HP_MIX :					
			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);	
			break;

		case FMR_DUAL_MIX :		
			//Mute the DAC path
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);
			
			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);	
			break;

		default:
			DEBUG_LOG_ERR("fmradio path[%d] is not invaild!\n", path);
			return;
			break;
		}
}

void wm8994_record_headset_mic(struct snd_soc_codec *codec) 
{
	struct wm8994_priv *wm8994 = codec->private_data;
	
	u16 val;

	audio_ctrl_mic_bias_gpio(1);
	
	DEBUG_LOG_INFO("Recording through Headset Mic\n");

	// Disable FM radio path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~WM8994_MIXINL_TO_SPKMIXL_MASK;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~WM8994_MIXINL_TO_MIXOUTL_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
	
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~WM8994_MIXINR_TO_MIXOUTR_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
	
	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	// Mixing left channel output to right channel.
	if(wm8994->recognition_active == REC_ON)
    {
    	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
		val &= ~(WM8994_AIF1ADCL_SRC_MASK | WM8994_AIF1ADCR_SRC_MASK);
		val |= (WM8994_AIF1ADCL_SRC | WM8994_AIF1ADCR_SRC);
		wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);
	}
	else
	{
		wm8994_write(codec, 0x0300, 0xC010);    // AIF1 Control 1. Right ADC data is only. I2S Format.
	}
	wm8994_write(codec,WM8994_ANTIPOP_2,0x68);	//Ear  mic volume issue fix
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);  
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);


	//Enable Right Input Mixer,Enable IN1R PGA - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2 );
//	val &= ~(WM8994_IN1R_ENA_MASK |WM8994_MIXINR_ENA_MASK  );
//	val |= (WM8994_MIXINR_ENA | WM8994_IN1R_ENA );
	val = (WM8994_MIXINR_ENA | WM8994_IN1R_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);

	if(!wm8994->testmode_config_flag)
	{	
		// Enable volume,unmute Right Line	
		val = wm8994_read(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1R_MUTE_MASK | WM8994_IN1R_VOL_MASK);	// Unmute IN1R
		if(wm8994->recognition_active == REC_ON)
			val |= (WM8994_IN1R_VU | TUNING_RECOGNITION_SUB_INPUTMIX_VOL);
		else
		val |= (WM8994_IN1R_VU | TUNING_RECORD_SUB_INPUTMIX_VOL);
		wm8994_write(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME,val);
		
		// unmute right pga, set volume 
		val = wm8994_read(codec,WM8994_INPUT_MIXER_4 );
		val&= ~(WM8994_IN1R_TO_MIXINR_MASK | WM8994_IN1R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		if(wm8994->recognition_active == REC_ON)
			val |= (WM8994_IN1R_TO_MIXINR | WM8994_IN1R_MIXINR_VOL);//30db
		else
		val |= (WM8994_IN1R_TO_MIXINR | WM8994_IN1R_MIXINR_VOL);//30db
		wm8994_write(codec,WM8994_INPUT_MIXER_4 ,val);
	}
	
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	if(wm8994->recognition_active == REC_ON)		
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_SUB_AIF1ADCL_VOL); 
	else
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_SUB_AIF1ADCL_VOL); // 0db
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

	val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1ADC1R_VOL_MASK);
	if(wm8994->recognition_active == REC_ON)		
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_SUB_AIF1ADCR_VOL);
	else
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_SUB_AIF1ADCR_VOL); // 0db
	wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);
	if(wm8994->recognition_active == REC_ON)
    {
		val = (WM8994_IN1RN_TO_IN1R | WM8994_IN1RP_TO_IN1R)  ; 
		wm8994_write(codec,WM8994_INPUT_MIXER_2,val);
	}
	else
	{
		wm8994_write(codec, 0x0028, 0x0001);    // Input Mixer 2. Single Ended Input. Negative is connected only.
	}

	
	//Digital Paths	
	//Enable right ADC and time slot
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_ADCR_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK );
	val |= (WM8994_AIF1ADC1R_ENA | WM8994_ADCR_ENA  );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);
	
	if(wm8994->recognition_active == REC_ON)
    {
		//ADC Right mixer routing
		val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
		val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK);
		val |= WM8994_ADC1R_TO_AIF1ADC1R;
		wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);
	}
	else
	{
		wm8994_write(codec, 0x0606, 0x0002);    // AIF1 ADC1 Left Mixer Routing. Enable.
		wm8994_write(codec, 0x0607, 0x0002);    // AIF1 ADC1 Right Mixer Routing. Enable.
	}

	val = wm8994_read(codec, WM8994_AIF1_MASTER_SLAVE);
	val |= (WM8994_AIF1_MSTR | WM8994_AIF1_CLK_FRC | WM8994_AIF1_LRCLK_FRC);	// Master mode
	wm8994_write(codec, WM8994_AIF1_MASTER_SLAVE, val);

	wm8994_write( codec, WM8994_GPIO_1, 0xA101 );   // GPIO1 is Input Enable
}

void wm8994_record_main_mic(struct snd_soc_codec *codec) 
{

struct wm8994_priv *wm8994 = codec->private_data;
	u16 val;
/*
    if(wm8994->codec_state & PLAYBACK_ACTIVE)
    {
        wm8994_disable_playback_path(codec, wm8994->cur_path);
        msleep(50);
    }
*/
	DEBUG_LOG("Recording through Main Mic\n");
	audio_ctrl_mic_bias_gpio(1);
	
	// Disable FM radio path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~WM8994_MIXINL_TO_SPKMIXL_MASK;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~WM8994_MIXINL_TO_MIXOUTL_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
	
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~WM8994_MIXINR_TO_MIXOUTR_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val); 
	
	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);
	
	if(wm8994->recognition_active == REC_ON)
	{
	    val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
		val &= ~(WM8994_AIF1ADCL_SRC_MASK | WM8994_AIF1ADCR_SRC_MASK);
		wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);	
	}
	else
	{
		wm8994_write(codec, 0x0300, 0x0010);    // AIF1 Control 1. Left ADC data is only. I2S Format.
	}
	wm8994_write(codec,WM8994_ANTIPOP_2,0x68);	//Main mic volume issue fix: requested H/W

		//Enable micbias,vmid,mic1
		val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
		val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK);
		val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);  
		wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);



	//Enable left input mixer and IN1L PGA	- Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2  );
//	val &= ~( WM8994_IN1L_ENA_MASK | WM8994_MIXINL_ENA_MASK );
//	val |= (WM8994_MIXINL_ENA |WM8994_IN1L_ENA );
	val = (WM8994_MIXINL_ENA |WM8994_IN1L_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);

	if(!wm8994->testmode_config_flag)
	{	
		if(wm8994->recognition_active == REC_ON)
        {
        	// Left Line Input 1&2 volume
            val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
			val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
			val |= (WM8994_IN1L_VU |TUNING_RECOGNITION_MAIN_INPUTLINE_VOL); //volume
			wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);    

			// Input Mixer 3
            val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
			val |= (WM8994_IN1L_TO_MIXINL);	// 0db
		    wm8994_write(codec, WM8994_INPUT_MIXER_3, val);  
			
			//Digital Paths
			val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
			val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_MAIN_AIF1ADCL_VOL);
			wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);// AIF1 ADC1 Left Volume

			val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
			val &= ~(WM8994_AIF1ADC1R_VOL_MASK);
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_MAIN_AIF1ADCR_VOL);
			wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);// AIF1 ADC1 Right Volume
        }
        else
        {
			wm8994_write(codec, 0x0018, (0x0140 | 0x001b));	// Left Line Input 1&2 volume
			wm8994_write(codec, 0x0029, WM8994_IN1L_TO_MIXINL);	  // Input Mixer 3
			wm8994_write(codec, 0x0400, (WM8994_AIF1ADC1_VU | TUNING_RECORD_MAIN_AIF1ADCL_VOL));	// AIF1 ADC1 Left Volume
			wm8994_write(codec, 0x0401, (WM8994_AIF1ADC1_VU | TUNING_RECORD_MAIN_AIF1ADCR_VOL));	// AIF1 ADC1 Right Volume
        }
	} 
	
	if(wm8994->recognition_active == REC_ON)
	{
		val = ( WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val);	
	}
	else
	{
		wm8994_write(codec, 0x0028, 0x0010);	// Input Mixer 2. Single Ended Input. Negative is connected only.
	}

	
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4 );
	val &= ~(WM8994_ADCL_ENA_MASK |WM8994_AIF1ADC1L_ENA_MASK  );
	val |= ( WM8994_AIF1ADC1L_ENA | WM8994_ADCL_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);

	if(wm8994->recognition_active == REC_ON)
	{
		//Enable timeslots
		val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING );
		val |=WM8994_ADC1L_TO_AIF1ADC1L ;  
		wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,val);
	}
	else
	{
		wm8994_write(codec, 0x0606, 0x0002);    // AIF1 ADC1 Left Mixer Routing. Enable.
		wm8994_write(codec, 0x0607, 0x0002);    // AIF1 ADC1 Right Mixer Routing. Enable.
	}

	
	val = wm8994_read(codec, WM8994_AIF1_MASTER_SLAVE);
	val |= (WM8994_AIF1_MSTR | WM8994_AIF1_CLK_FRC | WM8994_AIF1_LRCLK_FRC);	// Master mode
	wm8994_write(codec, WM8994_AIF1_MASTER_SLAVE, val);

	wm8994_write( codec, WM8994_GPIO_1, 0xA101 );   // GPIO1 is Input Enable

	if(wm8994->recognition_active == REC_ON)
	{
		/****************DRC START**********************/
		wm8994_write( codec, 0x444, 0x4); 
		wm8994_write( codec, 0x442, 0x800); 
		wm8994_write( codec, 0x450, 0x1BA); 
		/****************DRC END************************/
	}
}

void wm8994_record_bluetooth(struct snd_soc_codec *codec)
{
	u16 val;

	DEBUG_LOG_INFO("BT Record Path for Voice Command\n");

	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	//R1(1h) - 0x0003 -normal vmid
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);
	
	/*Digital Path Enables and Unmutes*/	
	//R2(2h) - 0x0000
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x0000);

	//R3(3h) - 0x0000
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0000);

	//R4(4h) - 0x0300
	//AIF1ADC1(Left/Right) Output
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK);
	val |= ( WM8994_AIF1ADC1L_ENA | WM8994_AIF1ADC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4 , val);

	//R5(5h) - 0x3000
	//AIF2DAC(Left/Right) Input
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);
	
	//R6(6h) - 0x0002
	//AIF2_DACDAT_SRC(GPIO8/DACDAT3)
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_6);
	val &= ~(WM8994_AIF2_DACDAT_SRC_MASK);
	val |= (WM8994_AIF2_DACDAT_SRC);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, val);

	/*Un-Mute*/
	//R1312(520h) - 0x0000
	//AIF2DAC_MUTE(Un-mute)
	val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);
	val &= ~(WM8994_AIF2DAC_MUTE_MASK);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val); 

	/*Mixer Routing*/
	//R1542(606h) - 0x0002
	//AIF2DACL_TO_AIF1ADC1L
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF2DACL_TO_AIF1ADC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);
	
	//R1543(607h) - 0x0002
	//AIF2DACR_TO_AIF1ADC1R
	val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF2DACR_TO_AIF1ADC1R_MASK);
	val |= (WM8994_AIF2DACR_TO_AIF1ADC1R);
	wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);

	wm8994_write(codec, WM8994_OVERSAMPLING, 0X0000);

	wm8994_set_voicecall_common_setting(codec);

	/*GPIO Configuration*/		
	wm8994_write(codec, WM8994_GPIO_8, WM8994_GP8_DIR | WM8994_GP8_DB);
	wm8994_write(codec, WM8994_GPIO_9, WM8994_GP9_DB);
	wm8994_write(codec, WM8994_GPIO_10, WM8994_GP10_DB);
	wm8994_write(codec, WM8994_GPIO_11, WM8994_GP11_DB); 
}

void wm8994_set_playback_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG_INFO("");
	
	if(!wm8994->testmode_config_flag)
	{
	        val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
	        val |= TUNING_RCV_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
	        wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
	
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_RCV_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU |WM8994_MIXOUTL_MUTE_N | TUNING_RCV_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RCV_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
	
	        val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK | WM8994_HPOUT2_VOL_MASK);
	        val |= TUNING_HPOUT2_VOL << WM8994_HPOUT2_VOL_SHIFT;
	        wm8994_write(codec,WM8994_HPOUT2_VOLUME, val );

		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL;
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}

	val = wm8994_read(codec,WM8994_OUTPUT_MIXER_1);
        val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
        val |= (WM8994_DAC1L_TO_MIXOUTL);
        wm8994_write(codec,WM8994_OUTPUT_MIXER_1,val);

        val = wm8994_read(codec,WM8994_OUTPUT_MIXER_2);
        val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
        val |= (WM8994_DAC1R_TO_MIXOUTR);
        wm8994_write(codec,WM8994_OUTPUT_MIXER_2,val);

	val = wm8994_read(codec,WM8994_HPOUT2_MIXER);
        val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK | WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
        val |= (WM8994_MIXOUTRVOL_TO_HPOUT2 | WM8994_MIXOUTLVOL_TO_HPOUT2);
        wm8994_write(codec,WM8994_HPOUT2_MIXER,val);

        val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);
        val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
        val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
        wm8994_write(codec,WM8994_POWER_MANAGEMENT_5,val);
/*
        val = wm8994_read(codec,WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK |WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
        wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1,val);
        */

	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);
        val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
        val |= (WM8994_AIF1DAC1L_TO_DAC1L);
        wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING,val);

        val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);
        val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
        val |= (WM8994_AIF1DAC1R_TO_DAC1R);
        wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING,val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);
        val &= ~(WM8994_DSP_FS1CLK_ENA_MASK | WM8994_DSP_FSINTCLK_ENA_MASK);
        val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
        wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
        val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
        val |= (WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTLVOL_ENA);
        wm8994_write(codec,WM8994_POWER_MANAGEMENT_3,val);
		
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK | WM8994_HPOUT2_ENA_MASK );
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_HPOUT2_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

	
	val = wm8994_read(codec,WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK |WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
        wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1,val);
}


void wm8994_set_playback_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;
	
	u16 TestReturn1=0;
	u16 TestReturn2=0;
	u16 TestLow1=0;
	u16 TestHigh1=0;
	u8 TestLow=0;
	u8 TestHigh=0;
	
	DEBUG_LOG_INFO("");

	//Configuring the Digital Paths


	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING ,val);
	

	//Enable the Timeslot0 to DAC1R
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= WM8994_AIF1DAC1R_TO_DAC1R;
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING ,val);

		val = wm8994_read(codec, 0x102  ); 	
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x102,val);
	
		val = wm8994_read(codec, 0x56  ); 	
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x56,val);
	
		val = wm8994_read(codec, 0x102  ); 	
		val &= ~(0x0000);
		val = 0x0000;
		wm8994_write(codec,0x102,val);

		val = wm8994_read(codec, WM8994_CLASS_W_1  ); 	
		val &= ~(0x0005);
		val |= 0x0005;
		wm8994_write(codec,WM8994_CLASS_W_1,val);

	// Headset Control
	if(!wm8994->testmode_config_flag)
	{

		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_RING_OUTPUTL_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_MP3_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);
	

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_RING_OUTPUTR_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_MP3_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_RING_OPGAL_VOL);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_MP3_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RING_OPGAR_VOL);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_MP3_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}
	
		val = wm8994_read(codec, WM8994_DC_SERVO_2  ); 	
		val &= ~(0x03E0);
		val = 0x03E0;
		wm8994_write(codec,WM8994_DC_SERVO_2,val);

	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL |WM8994_HPOUT1R_ENA	 |WM8994_HPOUT1L_ENA);  
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

		val = wm8994_read(codec, WM8994_ANALOGUE_HP_1  ); 	
		val &= ~(0x0022);
		val = 0x0022;
		wm8994_write(codec,WM8994_ANALOGUE_HP_1,val);

		//Enable Charge Pump	
		val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
		val &= ~WM8994_CP_ENA_MASK ;
		val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
		wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

		msleep(5);	// 20ms delay

	//Enable Dac1 and DAC2 and the Timeslot0 for AIF1	
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5 ); 	
	val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK |  WM8994_AIF1DAC1L_ENA_MASK );
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA  | WM8994_DAC1L_ENA |WM8994_DAC1R_ENA );
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5 ,val);


	// Enable DAC1L to HPOUT1L path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= WM8994_DAC1L_TO_MIXOUTL ;  	
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);	// Enable MIXOUT
	
	// Enable DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= WM8994_DAC1R_TO_MIXOUTR;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
	
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0030);
	
		val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
		val &= ~(0x0303);
		val = 0x0303;
		wm8994_write(codec,WM8994_DC_SERVO_1,val);
	
		msleep(160);	// 160ms delay

		TestReturn1=wm8994_read(codec,WM8994_DC_SERVO_4);
	
		TestLow=(signed char)(TestReturn1 & 0xff);
		TestHigh=(signed char)((TestReturn1>>8) & 0xff);

		TestLow1=((signed short)(TestLow-5))&0x00ff;
		TestHigh1=(((signed short)(TestHigh-5)<<8)&0xff00);
		TestReturn2=TestLow1|TestHigh1;
		wm8994_write(codec,WM8994_DC_SERVO_4, TestReturn2);

		val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
		val &= ~(0x000F);
		val = 0x000F;
		wm8994_write(codec,WM8994_DC_SERVO_1,val);

		msleep(20);

		// Intermediate HP settings
		val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
		val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
			WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
		val = (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
			WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
		wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= TUNING_DAC1L_VOL; 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= TUNING_DAC1R_VOL; //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);	
	
	// Unmute the AF1DAC1	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1 ); 	
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= WM8994_AIF1DAC1_UNMUTE;
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1 ,val);
	
//	wm8994->codec_HPAmp_state =HPAMP_PLAYBACK;
	
}

void wm8994_set_playback_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG_INFO("");
	wm8994_write(codec, 0x0420, 0x0200);    // AIF2 DAC Filter 1
	wm8994_write(codec, 0x0520, 0x0200);    // AIF2 DAC Filter 1

	//Disable end point for preventing pop up noise.
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_SPKLVOL_ENA_MASK);
	val |= WM8994_SPKLVOL_ENA;
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	// Speaker Volume Control
	if(!wm8994->testmode_config_flag)
	{
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
		
		val = wm8994_read(codec,WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_RING_SPKL_VOL);
		else
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec,WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		if(wm8994->ringtone_active)
			val |= TUNING_RING_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		else
			val |= TUNING_MP3_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);
	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}

	val = wm8994_read(codec,WM8994_SPKOUT_MIXERS);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= WM8994_SPKMIXL_TO_SPKOUTL;
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	//Unmute the DAC path
	val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= WM8994_DAC1L_TO_SPKMIXL;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	// Eable DAC1 Left and timeslot left
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);	
	val &= ~( WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | WM8994_DAC1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);   

	//Unmute
	/*
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);
	*/

	//enable timeslot0 to left dac
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);
	
	//Enbale bias,vmid and Left speaker
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_SPKOUTL_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);
}

void wm8994_set_playback_speaker_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	u16 nReadServo4Val = 0;
	u16 ncompensationResult = 0;
	u16 nCompensationResultLow=0;
	u16 nCompensationResultHigh=0;
	u8  nServo4Low = 0;
	u8  nServo4High = 0;

	//------------------  Common Settings ------------------
	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING ,val);
	
	//Enable the Timeslot0 to DAC1R
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= WM8994_AIF1DAC1R_TO_DAC1R;
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING ,val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

		//------------------  Speaker Path Settings ------------------

	// Speaker Volume Control
	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_RING_SPKL_VOL);
		else
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec,WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		if(wm8994->ringtone_active)
			val |= TUNING_RING_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		else
			val |= TUNING_MP3_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);
	}

	val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= WM8994_SPKMIXL_TO_SPKOUTL;
	wm8994_write(codec,WM8994_SPKOUT_MIXERS,val );

	//Unmute the DAC path
	val = wm8994_read(codec,WM8994_SPEAKER_MIXER );
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK );
	val |= WM8994_DAC1L_TO_SPKMIXL ;
	wm8994_write(codec,WM8994_SPEAKER_MIXER ,val);

	//------------------  Ear Path Settings ------------------
	//Configuring the Digital Paths
		val = wm8994_read(codec, 0x102);
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x102,val);
	
		val = wm8994_read(codec, 0x56  ); 	
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x56,val);
	
		val = wm8994_read(codec, 0x102  ); 	
		val &= ~(0x0000);
		val = 0x0000;
		wm8994_write(codec,0x102,val);

		val = wm8994_read(codec, WM8994_CLASS_W_1  ); 	
		val &= ~(0x0005);
		val = 0x0005;
		wm8994_write(codec,WM8994_CLASS_W_1,val);

	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_RING_DUAL_OUTPUTL_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_MP3_DUAL_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_RING_DUAL_OUTPUTR_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_MP3_DUAL_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

		//* DC Servo Series Count
		val = 0x03E0;
		wm8994_write(codec, WM8994_DC_SERVO_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |
		WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK|WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL |
		WM8994_HPOUT1R_ENA	 |WM8994_HPOUT1L_ENA |WM8994_SPKOUTL_ENA);
		wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

		val = (WM8994_HPOUT1L_DLY | WM8994_HPOUT1R_DLY);
		wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

		//Enable Charge Pump	
		val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
		val &= ~WM8994_CP_ENA_MASK ;
		val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
		wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

		msleep(5);

	//Enable DAC1 and DAC2 and the Timeslot0 for AIF1	
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5 );	
	val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK |  WM8994_AIF1DAC1L_ENA_MASK );
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA  | WM8994_DAC1L_ENA |WM8994_DAC1R_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_5,val);   

	// Enbale DAC1L to HPOUT1L path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |=  WM8994_DAC1L_TO_MIXOUTL;
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);

	// Enbale DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= WM8994_DAC1R_TO_MIXOUTR;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	//Enbale bias,vmid, hp left and right and Left speaker
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
	val |= (  WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3 ,val);

		//* DC Servo 
		val = (WM8994_DCS_TRIG_SERIES_1 | WM8994_DCS_TRIG_SERIES_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
		wm8994_write(codec,WM8994_DC_SERVO_1, 0x0303 );

		msleep(160);

		nReadServo4Val=wm8994_read(codec,WM8994_DC_SERVO_4);
		nServo4Low=(signed char)(nReadServo4Val & 0xff);
		nServo4High=(signed char)((nReadServo4Val>>8) & 0xff);
		
		nCompensationResultLow=((signed short)nServo4Low -5)&0x00ff;
		nCompensationResultHigh=((signed short)(nServo4High -5)<<8)&0xff00;
		ncompensationResult=nCompensationResultLow|nCompensationResultHigh;
		wm8994_write(codec,WM8994_DC_SERVO_4, ncompensationResult);

		val = (WM8994_DCS_TRIG_DAC_WR_1 | WM8994_DCS_TRIG_DAC_WR_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
		wm8994_write(codec,WM8994_DC_SERVO_1, val );

		msleep(15);	

		// Intermediate HP settings
		val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
		val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
			WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
		val |= (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
			WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
		wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	if(!wm8994->testmode_config_flag)
	{
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	}
//------------------  Common Settings ------------------
	// Unmute the AIF1DAC1	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1 ); 	
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1 ,val);
	if(!wm8994->testmode_config_flag)
	{
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);

		val = wm8994_read(codec,WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
	}
	
}                                                      

void wm8994_set_playback_bluetooth(struct snd_soc_codec *codec)
{
	u16 val;

	DEBUG_LOG_INFO("BT Record Path for Voice Command\n");

	//R1(1h) - 0x0003 -normal vmid
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);
	
	/*Digital Path Enables and Unmutes*/	
	//R2(2h) - 0x0000
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x0000);

	//R3(3h) - 0x0000
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0000);

	//R4(4h) - 0x3000
	//AIF2ADC(Left/Right) Output
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4 );
	val &= ~(WM8994_AIF2ADCL_ENA_MASK |WM8994_AIF2ADCR_ENA_MASK);
	val |= ( WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4 , val);

	//R5(5h) - 0x0300
	//AIF1DAC1(Left/Right) Input
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);
	
	//R6(6h) - 0x0008
	//AIF3_ADCDAT_SRC(AIF2ADCDAT2)
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_6);
	val &= ~(WM8994_AIF3_ADCDAT_SRC_MASK);
	val |= (0x0001 << WM8994_AIF3_ADCDAT_SRC_SHIFT);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, val);

	/*Un-Mute*/
	//R1056(420h) - 0x0000
	//AIF1DAC1_MUTE(Un-mute)
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);  

	/*Mixer Routing*/
	//R1540(604h) - 0x0004
	//AIF2DACL_TO_DAC2L
	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF2DACL_TO_DAC2L_MASK);
	val |= (WM8994_AIF2DACL_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);
	
	//R1541(605h) - 0x0004
	//AIF2DACR_TO_DAC2R
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF2DACR_TO_DAC2R_MASK);
	val |= (WM8994_AIF2DACR_TO_DAC2R);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	/*Volume*/
	wm8994_write(codec, WM8994_DAC2_LEFT_VOLUME, 0x01C0);
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, 0x01C0); 

	wm8994_write(codec, WM8994_OVERSAMPLING, 0X0000);

	wm8994_set_voicecall_common_setting(codec);

	/*GPIO Configuration*/		
	wm8994_write(codec, WM8994_GPIO_8, WM8994_GP8_DIR | WM8994_GP8_DB);
	wm8994_write(codec, WM8994_GPIO_9, WM8994_GP9_DB);
	wm8994_write(codec, WM8994_GPIO_10, WM8994_GP10_DB);
	wm8994_write(codec, WM8994_GPIO_11, WM8994_GP11_DB); 
}

void wm8994_set_playback_extra_dock_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG_INFO("");

	//OUTPUT mute
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK | WM8994_LINEOUT2P_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	
	if(!wm8994->testmode_config_flag)
	{
        val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
        val |= TUNING_EXTRA_DOCK_SPK_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
        wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
	
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_EXTRA_DOCK_SPK_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU |WM8994_MIXOUTL_MUTE_N | TUNING_MP3_EXTRA_DOCK_SPK_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_MP3_EXTRA_DOCK_SPK_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );

		val = wm8994_read(codec,WM8994_LINE_OUTPUTS_VOLUME);
		val &= ~(WM8994_LINEOUT2_VOL_MASK);
		val |= (TUNING_MP3_EXTRA_DOCK_SPK_VOL << WM8994_LINEOUT2_VOL_SHIFT);		
		wm8994_write(codec,WM8994_LINE_OUTPUTS_VOLUME,val);	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL;
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}

	val = wm8994_read(codec,WM8994_OUTPUT_MIXER_1);
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1,val);

	val = wm8994_read(codec,WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec,WM8994_OUTPUT_MIXER_2,val);


	val = wm8994_read(codec,WM8994_LINE_MIXER_2);
	val &= ~(WM8994_MIXOUTR_TO_LINEOUT2N_MASK | WM8994_MIXOUTL_TO_LINEOUT2N_MASK | WM8994_LINEOUT2_MODE_MASK | WM8994_MIXOUTR_TO_LINEOUT2P_MASK);
	val |= (WM8994_MIXOUTL_TO_LINEOUT2N | WM8994_LINEOUT2_MODE | WM8994_MIXOUTR_TO_LINEOUT2P);
	wm8994_write(codec,WM8994_LINE_MIXER_2,val);

	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_5,val);

	val = wm8994_read(codec,WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK |WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1,val);

	val = wm8994_read(codec,WM8994_LINE_OUTPUTS_VOLUME);
	val &= ~(WM8994_LINEOUT2N_MUTE_MASK | WM8994_LINEOUT2P_MUTE_MASK);
	wm8994_write(codec,WM8994_LINE_OUTPUTS_VOLUME,val);

	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING,val);

	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING,val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val &= ~(WM8994_DSP_FS1CLK_ENA_MASK | WM8994_DSP_FSINTCLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK | WM8994_LINEOUT1P_ENA_MASK | WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_LINEOUT2N_ENA | WM8994_LINEOUT2P_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTLVOL_ENA);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_3,val);
		
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK | WM8994_HPOUT2_ENA_MASK );
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

}

void wm8994_set_voicecall_common_setting(struct snd_soc_codec *codec)
{
	int val;

	/*GPIO Configuration*/
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);
	wm8994_write(codec, WM8994_GPIO_2, 0x8100);
	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);
	wm8994_write(codec, WM8994_GPIO_8, 0xA101);
	wm8994_write(codec, WM8994_GPIO_9, 0xA101);
	wm8994_write(codec, WM8994_GPIO_10, 0xA101);
	wm8994_write(codec, WM8994_GPIO_11, 0xA101);

	/*FLL2 Setting*/
	wm8994_write(codec, WM8994_FLL2_CONTROL_2, 0x2F00);
	wm8994_write(codec, WM8994_FLL2_CONTROL_3, 0x3126);
	wm8994_write(codec, WM8994_FLL2_CONTROL_4, 0x0100);
	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C88);
//	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C89);	// CP 24MHz
	wm8994_write(codec, WM8994_FLL2_CONTROL_1, WM8994_FLL2_FRACN_ENA | WM8994_FLL2_ENA);

	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);
	if(!(val & WM8994_AIF2CLK_ENA))
		wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0018);

	wm8994_write(codec, WM8994_AIF2_RATE, 0x3 << WM8994_AIF2CLK_RATE_SHIFT);
	
	// AIF2 Interface - PCM Stereo mode
	wm8994_write(codec, WM8994_AIF2_CONTROL_1,	//Left Justified, BCLK invert, LRCLK Invert
		WM8994_AIF2ADCR_SRC | WM8994_AIF2_BCLK_INV |0x18);

	wm8994_write(codec, WM8994_AIF2_BCLK, 0x70);
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, 0x0000);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, WM8994_AIF2_MSTR | WM8994_AIF2_CLK_FRC | WM8994_AIF2_LRCLK_FRC);	//Master

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	/*Clocking*/	
	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val |= (WM8994_DSP_FS2CLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x0);
	
	// AIF1 & AIF2 Output is connected to DAC1	
	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);	
	val |= (WM8994_AIF1DAC1L_TO_DAC1L | WM8994_AIF2DACL_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING, val );  

	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);	
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

	wm8994_write(codec, 0x6, 0x0);
}
void wm8994_set_playback_vps(struct snd_soc_codec *codec)
{
    /* Volume Setting */
    wm8994_write(codec, 0x0031, (TUNING_RCV_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT));    // Output Mixer 5
	wm8994_write(codec, 0x0032, (TUNING_RCV_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT));    // Output Mixer 6
	wm8994_write(codec, 0x0020, (WM8994_MIXOUT_VU |WM8994_MIXOUTL_MUTE_N | TUNING_RCV_OPGAL_VOL));    // Left OPGA Volume
	wm8994_write(codec, 0x0021, (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RCV_OPGAR_VOL));   // Right OPGA Volume
    wm8994_write(codec, 0x001E, 0x0000);    // Lineout Volume
	wm8994_write(codec, 0x0610, TUNING_DAC1L_VOL);    // DAC1 Left Volume
	wm8994_write(codec, 0x0611, TUNING_DAC1R_VOL);    // DAC 1 Right Volume
	wm8994_write(codec, 0x0402, (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL));    // AIF1 DAC 1 Left Volume
	wm8994_write(codec, 0x0403, (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL));    // AIF1 DAC 1 Rigt Volume

    /* Output Path Routing */
    wm8994_write(codec, 0x002D, 0x0001);    // Output Mixer1
    wm8994_write(codec, 0x002E, 0x0001);    // Output Mixer2
    wm8994_write(codec, 0x0035, 0x0021);    // Line Mixer 2
    wm8994_write(codec, 0x0005, 0x0303);    // Power Management5
    wm8994_write(codec, 0x0420, 0x0080);    // AIF1 DAC1 Filter
    wm8994_write(codec, 0x0601, 0x0001);    // DAC1 Left Mixer Routing
    wm8994_write(codec, 0x0602, 0x0001);    // DAC1 Right Mixer Routing
    wm8994_write(codec, 0x0208, 0x000A);    // Clocking 1
    wm8994_write(codec, 0x0003, 0x0CF0);    // Power Management 3

    DEBUG_LOG_INFO("");
}


void wm8994_set_voicecall_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;
	
	DEBUG_LOG("");

	audio_ctrl_mic_bias_gpio(1);
	
    wm8994_write(codec, 0x0039, 0x0068);    // Anti Pop2
    wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
    wm8994_write(codec, 0x0015, 0x0040);
    wm8994_write(codec, 0x0702, 0x8100);    // GPIO 3. Speech PCM Clock
    wm8994_write(codec, 0x0703, 0x8100);    // GPIO 4. Speech PCM Sync
    wm8994_write(codec, 0x0704, 0x8100);    // GPIO 5. Speech PCM Data Out
    wm8994_write(codec, 0x0706, 0x0100);    // GPIO 7. Speech PCM Data Input
    wm8994_write(codec, 0x0244, 0x0C81);    // FLL2 Control 5
    wm8994_write(codec, 0x0241, 0x2F00);    // FLL2 Control 2
    wm8994_write(codec, 0x0243, 0x0600);    // FLL2 Control 4
    wm8994_write(codec, 0x0240, 0x0001);    // FLL2 Control 1
    wm8994_write(codec, 0x0208, 0x000F);    // Clocking 1. '0x000A' is added for a playback. (original = 0x0007)

    wm8994_write(codec, 0x0204, 0x0009);    // AIF2 Clocking 1
    wm8994_write(codec, 0x0211, 0x0003);    // AIF2 Rate
    wm8994_write(codec, 0x0310, 0x4118);    // AIF2 Control 1
    wm8994_write(codec, 0x0311, 0x0000);    // AIF2 Control 2 pcm format is changed ulaw to linear
    wm8994_write(codec, 0x0520, 0x0000);    // AIF2 DAC Filter 1
    wm8994_write(codec, 0x0601, 0x0005);    // DAC1 Left Mixer Routing
    wm8994_write(codec, 0x0602, 0x0001);    //  DAC1 Right Mixer Routing(Playback)
    wm8994_write(codec, 0x0603, 0x018C);    // DAC2 Mixer Volumes
    wm8994_write(codec, 0x0604, 0x0030);    // DAC2 Left Mixer Routing
    wm8994_write(codec, 0x0605, 0x0010);    // DAC2 Right Mixer Routing
    wm8994_write(codec, 0x0621, 0x01C0);    // Sidetone
    wm8994_write(codec, 0x0620, 0x0000);    // Oversampling
    //sub mic
    wm8994_write(codec, 0x0002, 0x6240);    // Power Management 2
    //sub mic
    wm8994_write(codec, 0x0028, 0x0030);    // Input Mixer 2 30 -> 00
    
    if(!wm8994->testmode_config_flag)
    {
        /* Left Line Input 1&2 Volume */
        val = wm8994_read(codec, 0x0018);
        val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
        val |= (0x0100 | TUNING_CALL_RCV_INPUTMIX_VOL);
		//sub mic
	   wm8994_write(codec, 0x0018, 0x010A);		
       
	   
        /* Output Mixer 5 */
        val = wm8994_read(codec, 0x0031);
        val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
        val |= 0x0000 << 0x0009;
        wm8994_write(codec,0x0031, val);

        /* Output Mixer 6 */
        val = wm8994_read(codec, 0x0032);
        val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
        val |= 0x0000 << 0x0009;
        wm8994_write(codec,0x0032, val);

        /* Left OPGA Volume */
        val = wm8994_read(codec, 0x0020);
        val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
        val |= (0x0100 | 0x0040 | TUNING_RCV_OPGAL_VOL);
        wm8994_write(codec,0x0020, 0x01F9);//05.24 Maximum ��� -6dB HAC �� test -2 3B -> 39
        

        /* Right OPGA Volume */
        val = wm8994_read(codec, 0x0021);
        val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
        val |= (0x0100 | 0x0040 | TUNING_RCV_OPGAR_VOL);
        wm8994_write(codec,0x0021, 0x01F9);

    }

    wm8994_write(codec, 0x0312, 0x0000);    // Slave
    //sub mic
    wm8994_write(codec, 0x0029, 0x0030);    // Input Mixer 3
   
    
    wm8994_write(codec, 0x0015, 0x0000);    
    wm8994_write(codec, 0x0500, 0x0100);    // AIF2 ADC Left Volume
    wm8994_write(codec, 0x0004, 0x2002);    // Power Management 4
    wm8994_write(codec, 0x0005, 0x2303);    // Power Management 5. '0x0303' is added for a playback. (Original = 0x2002)
    wm8994_write(codec, 0x0003, 0x00F0);    // Power Management 3. '0x00F0' is added for a playback. (Original = 0x00A0)
    wm8994_write(codec, 0x002D, 0x0001);    // Output Mixer 1
    wm8994_write(codec, 0x002E, 0x0001);    // Output Mixer 2(Playback)
    wm8994_write(codec, 0x0033, 0x0018);    // HPOUT2 Mixer. '0x0008' is added for a playback. (Original = 0x0010)
    wm8994_write(codec, 0x0038, 0x0040);    // Anti Pop 1
    wm8994_write(codec, 0x0420, 0x0080);    // AIF1 DAC1 FIlter(Playback)

    if(!wm8994->testmode_config_flag)
    {
        /* HPOUT2 Volume */
#if 0
				wm8994_write(codec, 0x001F, (TUNING_HPOUT2_VOL << 0x0004));    // HPOUT2 Volume
#else
				if(wm8994->codec_state & CALL_ACTIVE)
				{
					wm8994_write(codec, 0x001F, (TUNING_HPOUT2_VOL << 0x0004));    // HPOUT2 Volume
				}
				else
				{
					wm8994_write(codec, 0x001F, 0x0020);	// HPOUT2 Volume
				}
#endif

    }
    
    wm8994_write(codec, 0x0001, 0x0803);    // Power Management 1

    if(!wm8994->testmode_config_flag)
    {
        /* DAC1 Left Volume */
        val = wm8994_read(codec,0x0610);
        val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
        val |= TUNING_DAC1L_VOL; 
        wm8994_write(codec, 0x0610, 0x01C0);

        /* DAC1 Right Volume */
        val = wm8994_read(codec, 0x0611); 
        val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
        val |= TUNING_DAC1R_VOL;
        wm8994_write(codec, 0x0611, 0x01C0);

        /* Power Management 3(Playback) */
        wm8994_write(codec, 0x0003, 0x00F0);
    }
    wm8994_write(codec,0x06, 0x0000 );
    wm8994_write(codec, 0x0612, 0x01C0);    // DAC2 Left Volume
    wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume
    wm8994_write(codec, 0x0500, 0x01C0);    // AIF2 ADC Left Volume

}


void wm8994_set_voicecall_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;
    int val;
    extern int hw_rev_adc;
    extern int victory_get_revision_adc(int adc_ch);
	hw_rev_adc =  victory_get_revision_adc(7);
	//tty_mode = TTY_MODE_HCO;
	
    


#if 1
	wm8994_write(codec, 0x0039, 0x006C);    // Anti Pop 2
    wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
    msleep(50);
    wm8994_write(codec, 0x0102, 0x0003);    // To remove the robotic sound
    wm8994_write(codec, 0x0817, 0x0000);    // To remove the robotic sound
    wm8994_write(codec, 0x0102, 0x0003);    // To remove the robotic sound
    wm8994_write(codec, 0x004C, 0x9F25);    // Charge Pump 1

    wm8994_write(codec, 0x0702, 0x8100);    // GPIO 3. Speech PCM Clock
    wm8994_write(codec, 0x0703, 0x8100);    // GPIO 4. Speech PCM Sync
    wm8994_write(codec, 0x0704, 0x8100);    // GPIO 5. Speech PCM Data Out
    wm8994_write(codec, 0x0706, 0x0100);    // GPIO 7. Speech PCM Data Input

    wm8994_write(codec, 0x0244, 0x0C81);    // FLL2 Control 5
    wm8994_write(codec, 0x0241, 0x0700);    // FLL2 Control 2
    wm8994_write(codec, 0x0242, 0x0000);    // FLL2 Control 3
    wm8994_write(codec, 0x0243, 0x0600);    // FLL2 Control 4
    wm8994_write(codec, 0x0240, 0x0001);    // FLL2 Control 1
    msleep(3);

    /* Audio Interface & Clock Setting */
    wm8994_write(codec, 0x0204, 0x0018);    // AIF2 Clocking 1
    wm8994_write(codec, 0x0208, 0x000F);    // Clocking 1. '0x000A' is added for a playback. (original = 0x0007)
    wm8994_write(codec, 0x0620, 0x0000);    // Oversampling
    wm8994_write(codec, 0x0211, 0x0009);    // AIF2 Rate
    wm8994_write(codec, 0x0302, 0x4000);    // AIF1 Master Slave Setting. To prevent that the music is played slowly.
    wm8994_write(codec, 0x0312, 0x0000);    // AIF2 Master Slave Setting

    if(tty_mode == TTY_MODE_VCO)
    {
        wm8994_write(codec, 0x0310, 0x4118);    // AIF2 Control 1
    }
    else
    {
        wm8994_write(codec, 0x0310, 0xC118);    // AIF2 Control 1
    }
    wm8994_write(codec, 0x0311, 0x0000);    // AIF2 Control 2
    //wm8994_write(codec, 0x0520, 0x0080);    // AIF2 DAC Filter1
    wm8994_write(codec, 0x0204, 0x0019);    // AIF2 Clocking 1

    /* Input Path Routing */
    if(tty_mode == TTY_MODE_VCO)
    {
        wm8994_write(codec, 0x0028, 0x0030);    // Input Mixer 2
        wm8994_write(codec, 0x0002, 0x6240);    // Power Management 2
        wm8994_write(codec, 0x0029, 0x0030);    // Input Mixer 
        wm8994_write(codec, 0x0004, 0x2002);    // Power Management 4
    }
    else if(tty_mode == TTY_MODE_HCO || tty_mode == TTY_MODE_FULL)
    {
        wm8994_write(codec, 0x0028, 0x0001);    // Input Mixer 2
        wm8994_write(codec, 0x0002, 0x6130);    // Power Management 2
        wm8994_write(codec, 0x002A, 0x0030);    // Input Mixer 4
        wm8994_write(codec, 0x0004, 0x1001);    // Power Management 4
    }
    else
    {
        wm8994_write(codec, 0x0028, 0x0001);    // Input Mixer 2
        wm8994_write(codec, 0x0002, 0x6130);    // Power Management 2
        wm8994_write(codec, 0x002A, 0x0037);    // Input Mixer 4
        wm8994_write(codec, 0x0004, 0x1001);    // Power Management 4
    }
    wm8994_write(codec, 0x0604, WM8994_ADC2_TO_DAC2L | WM8994_ADC1_TO_DAC2L);    // DAC2 Left Mixer Routing
    wm8994_write(codec, 0x0605, WM8994_ADC2_TO_DAC2L | WM8994_ADC1_TO_DAC2L);    // DAC2 Right Mixer Routing

	gpio_set_value(S5PC11X_GPB(1), 1);
	audio_ctrl_mic_bias_gpio(1);
    //gpio_set_value(S5PC11X_GPB(1), 1);

    /* Output Path Routing */
    wm8994_write(codec, 0x0005, 0x3303);    // Power Management 5

    if(tty_mode == TTY_MODE_HCO)
    {
        wm8994_write(codec, 0x0601, 0x0005);    // DAC1 Left Mixer Routing
        wm8994_write(codec, 0x0602, 0x0005);    // DAC1 Right Mixer Routing
        wm8994_write(codec, 0x002D, 0x0001);    // Output Mixer 1
        wm8994_write(codec, 0x002E, 0x0001);    // Output Mixer 2
        wm8994_write(codec, 0x0003, 0x00F0);    // Power Management 3
        wm8994_write(codec, 0x0033, 0x0018);    // HPOUT2 Mixer
        wm8994_write(codec, 0x0420, 0x0080);    // AIF1 DAC1 Filter1
    }
    else
    {
        wm8994_write(codec, 0x0601, 0x0005);    // DAC1 Left Mixer Routing
        wm8994_write(codec, 0x0602, 0x0005);    // DAC1 Right Mixer Routing
        wm8994_write(codec, 0x002D, 0x0100);    // Output Mixer 1
        wm8994_write(codec, 0x002E, 0x0100);    // Output Mixer 2
        wm8994_write(codec, 0x0003, 0x00F0);    // Power Management 3(Playback)
        wm8994_write(codec, 0x0060, 0x00EE);    // Analogue HP 1
        //wm8994_write(codec, 0x0420, 0x0080);    // AIF1 DAC1 Filter1
    }

    /* Input Path Volume */
    if(!wm8994->testmode_config_flag)
    {
        if(tty_mode == TTY_MODE_VCO)
        {
            val = wm8994_read(codec, 0x18);	
            val &= ~(0x0080 | 0x001F);
            val |= (0x0100 |TUNING_RECORD_MAIN_INPUTLINE_VOL); //volume
            wm8994_write(codec, 0x18, val);
            wm8994_write(codec,0x612, 0x01C0 );    // DAC2 Left Volume
        }		
        else if(tty_mode == TTY_MODE_HCO || tty_mode == TTY_MODE_FULL)
        {
        	val = wm8994_read(codec,0x1A);	
        	val &= ~(0x0080 | 0x001F);	// Unmute IN1R
			val |= (0x0100 | 0x08); 
			wm8994_write(codec,0x1A,val);    // Right Line Input 1&2 Volume
            wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume
        }
        else
        {
        	val = wm8994_read(codec,0x1A);	
        	val &= ~(0x0080 | 0x001F);	// Unmute IN1R
            if(loopback_mode)
                val |= (0x0100 | 0x03);
            else
        	val |= (0x0100 | 0x17);
            wm8994_write(codec,0x1A,val);   // Right Line Input 1&2 Volume
            wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume
			//wm8994_write(codec,0x500, 0x88);  
			//wm8994_write(codec,0x501, 0x88);      // AIF2 Right ADC Volume
        }
        wm8994_write(codec,0x603, 0x018C );     // DAC2 Mixer Volumes
    }

    /* Output Path Volume */
    if(!wm8994->testmode_config_flag)
    {
        if(tty_mode == TTY_MODE_HCO)
        {
            val = wm8994_read(codec, 0x31);
            val &= ~(0x0E00);
            val |= 0x0 << 9;
            wm8994_write(codec,0x31, val );
			

            val = wm8994_read(codec, 0x32);
            val &= ~(0x0E00);
            val |= 0x0 << 9;
            wm8994_write(codec,0x32, val );            

            val = wm8994_read(codec, 0x20);
            val &= ~(0x0040 | 0x003F);
            val |= (0x0100 |0x0040 | TUNING_RCV_OPGAL_VOL);
            wm8994_write(codec,0x20, val );
            

            val = wm8994_read(codec, 0x21);
            val &= ~(0x0040 | 0x003F);
            val |= (0x0100 | 0x0040 | TUNING_RCV_OPGAR_VOL);
            wm8994_write(codec,0x21, val );
            //Unmute DAC1 left
            val = wm8994_read(codec,0x610 );
            val &= ~(0x0200 | 0x00FF);
            val |= TUNING_DAC1L_VOL; 
            wm8994_write(codec,0x610 ,val);
            
            //Unmute and volume ctrl RightDAC
            val = wm8994_read(codec, 0x611 ); 
            val &= ~(0x0200 | 0x00FF);
            val |= TUNING_DAC1R_VOL;
            wm8994_write(codec,0x611,val);
#if 0
            wm8994_write(codec, 0x001F, (TUNING_HPOUT2_VOL << 0x0004));    // HPOUT2 Volume
#else
            if(wm8994->codec_state & CALL_ACTIVE)
            {
                wm8994_write(codec, 0x001F, (TUNING_HPOUT2_VOL << 0x0004));    // HPOUT2 Volume
            }
            else
            {
                wm8994_write(codec, 0x001F, 0x0020);    // HPOUT2 Volume
            }
#endif
        }
        else
        {
            wm8994_write(codec, 0x0031, (TUNING_EAR_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT));    // Output Mixer 5
            wm8994_write(codec, 0x0032, (TUNING_EAR_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT));    // Outupt Mixer 6
            wm8994_write(codec, 0x0020, (0x0140 | TUNING_CALL_OPGAL_VOL));    // Left OPGA Volume
            wm8994_write(codec, 0x0021, (0x0140 | TUNING_CALL_OPGAR_VOL));    // Right OPGA Volume
#if 0
            wm8994_write(codec, 0x001C, (0x0140 | TUNING_CALL_OUTPUTL_VOL));    // Left Output Volume
            wm8994_write(codec, 0x001D, (0x0140 | TUNING_CALL_OUTPUTR_VOL));    // Right Output Volume
#else
			if(tty_mode == TTY_MODE_FULL)
				wm8994_write(codec, 0x1C, (0x0100 | 0x0040 | 0x1A));
			else{

	            if(wm8994->codec_state & CALL_ACTIVE)
	            {
	               wm8994_write(codec, 0x001C, (0x0140 | TUNING_CALL_OUTPUTL_VOL));    // Left Output Volume
                   wm8994_write(codec, 0x001D, (0x0140 | TUNING_CALL_OUTPUTR_VOL));    // Right Output Volume
	            }
	            else
	            {
	                wm8994_write(codec, 0x001C, 0x0100);    // Left Output Volume
                    wm8994_write(codec, 0x001D, 0x0100);    // Right Output Volume
	            }
			}
#endif
            //Unmute DAC1 left
            wm8994_write(codec, 0x0610, (0x0100 | TUNING_DAC1L_VOL));    // DAC1 Left Volume
            wm8994_write(codec, 0x0611, (0x0100 | TUNING_DAC1R_VOL));    // DAC1 Right Volume
        }
    }
    wm8994_write(codec, 0x0006, 0x0000);    // Power Management 6. Prevent the mute when the audio transfer is executed from the bluetooth.
    
    if(tty_mode == TTY_MODE_HCO)
    {
        wm8994_write(codec, 0x0038, 0x0040);    // Anti Pop 1
    }            
    wm8994_write(codec, 0x0621, 0x01C0);    // Sidetone

    if(tty_mode == TTY_MODE_HCO)
    {
        wm8994_write(codec, 0x0001, 0x0833);    // Power Management 1
    }
    else
    {
        wm8994_write(codec, 0x0001, 0x0303);    // Power Management 1
    }
	wm8994_write(codec, 0x0054, 0x0303);
	msleep(160);
	wm8994_write(codec, 0x0057, 0x040C);
	wm8994_write(codec, 0x0054, 0x000F);
	msleep(20);
    wm8994_write(codec, 0x0224, 0x0C88);    // FLL1 Control(5). To set again the sampling rate for a AP sound.
    wm8994_write(codec, 0x0520, 0x0080);    // AIF2 DAC Filter1
    wm8994_write(codec, 0x0420, 0x0080);    // AIF1 DAC1 Filter1

#else
    /* Power - UP seqeunce- COLD Start Up*/
    wm8994_write(codec,0x39, 0x006C );  
    wm8994_write(codec,0x01, 0x0003 );   
	msleep(50);
    wm8994_write(codec,0x4C, 0x9F25 );  
	
	wm8994_write(codec, 0x0702, 0x8100);	// GPIO 3. Speech PCM Clock
	wm8994_write(codec, 0x0703, 0x8100);	// GPIO 4. Speech PCM Sync
	wm8994_write(codec, 0x0704, 0x8100);	// GPIO 5. Speech PCM Data Out
	wm8994_write(codec, 0x0706, 0x0100);	// GPIO 7. Speech PCM Data Input

    if(tty_mode == TTY_MODE_HCO)
    {
        wm8994_write(codec,0x01, 0x0833 );
    }
    else
    {
        wm8994_write(codec,0x01, 0x0303 );  
    }
    wm8994_write(codec,0x60, 0x0022 );  
    wm8994_write(codec,0x60, 0x00EE );  
    
    /*Analogue Input Configuration*/
    if(tty_mode == TTY_MODE_VCO)
    {
        wm8994_write(codec, 0x02, 0x6240);
        wm8994_write(codec, 0x28, 0x0030);
    }
    else
    {
        wm8994_write(codec, 0x02, 0x6110);
        wm8994_write(codec, 0x28, 0x0003);
    }    
    /*Analogue Output Configuration*/	
    if(tty_mode == TTY_MODE_HCO)
    {
        wm8994_write(codec,0x2D, 0x0001);
        wm8994_write(codec,0x2E, 0x0001);
        wm8994_write(codec, 0x0038, 0x0040);
        wm8994_write(codec,0x33, 0x18);
    }
    else
    {
        wm8994_write(codec,0x2D, 0x0100 );   
        wm8994_write(codec,0x2E, 0x0100 );
    }
    wm8994_write(codec,0x4C, 0x9F25 );   
    wm8994_write(codec,0x60, 0x00EE ); 
    
    /*Digital Path Enables and Unmutes*/	
    if(tty_mode == TTY_MODE_VCO)
   {
        wm8994_write(codec,0x04, 0x2002 );
   }
    else
    {
        wm8994_write(codec,0x04, 0x3003 );
    }

	audio_ctrl_mic_bias_gpio(1);
    //gpio_set_value(S5PC11X_GPB(1), 1);

	
    wm8994_write(codec,0x05, 0x3303 );
    wm8994_write(codec,0x06, 0x0000 );
    wm8994_write(codec,0x420, 0x0080 );  
    wm8994_write(codec,0x520, 0x0080 );

    if(tty_mode == TTY_MODE_HCO)
    {
        val = wm8994_read(codec,0x601);
        val &= ~(0x0001);
        val |= (0x0001);
        wm8994_write(codec,0x601, 0x0005 );     

        val = wm8994_read(codec,0x602);
        val &= ~(0x0001);
        val |= (0x0001);
        wm8994_write(codec,0x602, 0x0005 );
    }
    else
    {
        wm8994_write(codec,0x601, 0x0005 );  
        wm8994_write(codec,0x602, 0x0005 );
    }
    wm8994_write(codec,0x612, 0x01C0 );
    wm8994_write(codec,0x613, 0x01C0 );
    
    if(wm8994->hw_version == 3)    // H/W Rev D
    //if(wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING) == 3)
    {
        wm8994_write(codec, 0x604, 0x0020 | 0x0010);
        wm8994_write(codec,0x603, 0x018C );  
        wm8994_write(codec,0x621, 0x01C0);
    }
    else    // H/W Rev B
    {
        wm8994_write(codec,0x603, 0x000C );  
        wm8994_write(codec, 0x604, WM8994_ADC1_TO_DAC2L);
        wm8994_write(codec,0x621, 0x01C1 );
    }
    
    /*FLL2  Setting*/	
    wm8994_write(codec,0x244, 0x0C81 ); //BCLK2  
    wm8994_write(codec,0x241, 0x2F00 ); //48  
    wm8994_write(codec,0x243, 0x0600 );   
    wm8994_write(codec,0x240, 0x0001 ); //FLL2 Enable, Integer Mode
    
    /*Clocking*/		
    wm8994_write(codec,0x208, 0x0007 );  
    
    /*AIF2 Interface*/	
    wm8994_write(codec,0x204, 0x0009 ); //FLL2 En, AIF2 Clock Enable.   
    wm8994_write(codec,0x211, 0x0003 );   
    wm8994_write(codec,0x310, 0x4118 ); //DSP Mode 
    wm8994_write(codec,0x311, 0x0000 ); //u_Law Companding pcm format is changed ulaw to linear
    
    /*GPIO Configuration
    //wm8994_write(codec,WM8994_GPIO_1, 0xA101 );  
    wm8994_write(codec,0x701, 0xA101 );  
    wm8994_write(codec,0x702, 0x8100 );  
    wm8994_write(codec,0x703, 0x8100 );  
    wm8994_write(codec,0x704, 0x8100 );  
    wm8994_write(codec,0x705, 0xA101 );  
    wm8994_write(codec,0x706, 0x0100 );  
	*/	

    //mic gain up
    wm8994_write(codec,0x500, 0x88);  
    wm8994_write(codec,0x501, 0x88);  
    
    /* Unmute*/
    //if(!wm8994->testmode_config_flag)
    { 
        if(tty_mode == TTY_MODE_VCO)
        {
            val = wm8994_read(codec, 0x18);	
            val &= ~(0x0080 | 0x001F);
            val |= (0x0100 |TUNING_RECORD_MAIN_INPUTLINE_VOL); //volume
            wm8994_write(codec, 0x18, val);
        
            //Unmute the PGA
            val = wm8994_read(codec, 0x29);
            val&= ~(0x0020 | 0x0010 | 0x0007);
            val |= (0x0010 | 0x0020);	//30db
            wm8994_write(codec, 0x29, val); 
        }
        else
        {
            val = wm8994_read(codec,0x1A);	
            val &= ~(0x0080 | 0x001F);	// Unmute IN1R
            if( 1000 <= hw_rev_adc && hw_rev_adc <= 1450){
				val |= (0x0100 | 0x0B); 
			}
            else{
				if(tty_mode == TTY_MODE_FULL)
            		val |= (0x0100 | 0x0A); 
				else					
					val |= (0x0100 | 0x17); 
            }
            wm8994_write(codec,0x1A,val);
        
            // unmute right pga, set volume 
            val = wm8994_read(codec,0x2A );
            val&= ~(0x0020 | 0x0010 | 0x0007);
			if(tty_mode == TTY_MODE_FULL)
            		val |= (0x0020 | 0x10);//0db
				else					
					val |= (0x0020 | 0x17);//0db
            wm8994_write(codec,0x2A ,val);

        }
        // Volume Control - Output

        if(tty_mode == TTY_MODE_HCO)
        {
            val = wm8994_read(codec, 0x31);
            val &= ~(0x0E00);
            val |= 0x0 << 9;
            wm8994_write(codec,0x31, val );
			

            val = wm8994_read(codec, 0x32);
            val &= ~(0x0E00);
            val |= 0x0 << 9;
            wm8994_write(codec,0x32, val );            

            val = wm8994_read(codec, 0x20);
            val &= ~(0x0040 | 0x003F);
            val |= (0x0100 |0x0040 | TUNING_RCV_OPGAL_VOL);
            wm8994_write(codec,0x20, val );
            

            val = wm8994_read(codec, 0x21);
            val &= ~(0x0040 | 0x003F);
            val |= (0x0100 | 0x0040 | TUNING_RCV_OPGAR_VOL);
            wm8994_write(codec,0x21, val );

            
            #if 0
            wm8994_write(codec, 0x001F, (TUNING_HPOUT2_VOL << 0x0004));    // HPOUT2 Volume
#else
            if(wm8994->codec_state & CALL_ACTIVE)
            {
                wm8994_write(codec, 0x001F, (TUNING_HPOUT2_VOL << 0x0004));    // HPOUT2 Volume
            }
            else
            {
                wm8994_write(codec, 0x001F, 0x0020);    // HPOUT2 Volume
            }
#endif

            //Unmute DAC1 left
            val = wm8994_read(codec,0x610 );
            val &= ~(0x0200 | 0x00FF);
            val |= TUNING_DAC1L_VOL; 
            wm8994_write(codec,0x610 ,val);
            
            //Unmute and volume ctrl RightDAC
            val = wm8994_read(codec, 0x611 ); 
            val &= ~(0x0200 | 0x00FF);
            val |= TUNING_DAC1R_VOL;
            wm8994_write(codec,0x611,val);
            

            val = wm8994_read(codec,0x03);
            val &= ~(0x0080 | 0x0040 | 0x0020 | 0x0010);
            val |= (0x0020 | 0x0010 | 0x0040 | 0x0080);
            wm8994_write(codec,0x03,val);
            }
        else
            {
            val = wm8994_read(codec, 0x31);
            val &= ~(0x0E00);
            val |= TUNING_EAR_OUTMIX5_VOL << 9;
            wm8994_write(codec,0x31, val );
            

            val = wm8994_read(codec, 0x32);
            val &= ~(0x0E00);
            val |= TUNING_EAR_OUTMIX6_VOL << 9;
            wm8994_write(codec,0x32, val );
            

			if(tty_mode == TTY_MODE_FULL)
				wm8994_write(codec, 0x1C, (0x0100 | 0x0040 | 0x1A));
			else{
#if 0
            wm8994_write(codec, 0x001C, (0x0140 | TUNING_CALL_OUTPUTL_VOL));    // Left Output Volume
            wm8994_write(codec, 0x001D, (0x0140 | TUNING_CALL_OUTPUTR_VOL));    // Right Output Volume
#else
            if(wm8994->codec_state & CALL_ACTIVE)
            {
                wm8994_write(codec, 0x001C, (0x0140 | TUNING_CALL_OUTPUTL_VOL));    // Left Output Volume
                wm8994_write(codec, 0x001D, (0x0140 | TUNING_CALL_OUTPUTR_VOL));    // Right Output Volume
            }
            else
            {
                wm8994_write(codec, 0x001C, 0x0100);    // Left Output Volume
                wm8994_write(codec, 0x001D, 0x0100);    // Right Output Volume
            }
#endif

			}            

            val = wm8994_read(codec, 0x20);
            val &= ~(0x0040 | 0x003F);
            val |= (0x0100 | 0x0040 | TUNING_CALL_OPGAL_VOL);
            wm8994_write(codec,0x20, val );

            
            val = wm8994_read(codec, 0x21);
            val &= ~(0x0040 | 0x003F);
            val |= (0x0100 | 0x0040 | TUNING_CALL_OPGAR_VOL);
            wm8994_write(codec,0x21, val );
            
            //Unmute DAC1 left
            val = wm8994_read(codec,0x610 );
            val &= ~(0x0200 | 0x00FF);
            val |= TUNING_DAC1L_VOL; 
            wm8994_write(codec,0x610 ,val);
            
            //Unmute and volume ctrl RightDAC
            val = wm8994_read(codec, 0x611 ); 
            val &= ~(0x0200 | 0x00FF);
            val |= TUNING_DAC1R_VOL; //0 db volume	
            wm8994_write(codec,0x611,val);
        }
    }

#endif
}

void wm8994_set_voicecall_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;
	int val;
#if 0
wm8994_write(codec, 0x0039, 0x006C);    // Anti Pop 2
	wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
	msleep(50);
    wm8994_write(codec, 0x0102, 0x0003);    // To remove the robotic sound
    wm8994_write(codec, 0x0817, 0x0000);    // To remove the robotic sound
    wm8994_write(codec, 0x0102, 0x0003);    // To remove the robotic sound

    wm8994_write(codec, 0x0702, 0x8100);    // GPIO 3. Speech PCM Clock
	wm8994_write(codec, 0x0703, 0x8100);    // GPIO 4. Speech PCM Sync
	wm8994_write(codec, 0x0704, 0x8100);    // GPIO 5. Speech PCM Data Out
	wm8994_write(codec, 0x0706, 0x0100);    // GPIO 7. Speech PCM Data Input

    wm8994_write(codec, 0x0244, 0x0C81);    // FLL2 Control 5
    wm8994_write(codec, 0x0241, 0x0700);    // FLL2 Control 2
    wm8994_write(codec, 0x0242, 0x0000);    // FLL2 Control 3
    wm8994_write(codec, 0x0243, 0x0600);    // FLL2 Control 4
    wm8994_write(codec, 0x0240, 0x0001);    // FLL2 Control 1
    msleep(3);

    /* Audio Interface & Clock Setting */
    wm8994_write(codec, 0x0204, 0x0018);    // AIF2 Clocking 1
    wm8994_write(codec, 0x0208, 0x000F);    // Clocking 1. '0x000A' is added for a playback. (original = 0x0007)
    wm8994_write(codec, 0x0620, 0x0000);    // Oversampling
    wm8994_write(codec, 0x0211, 0x0009);    // AIF2 Rate
    wm8994_write(codec, 0x0300, 0x0010);    // AIF1 Control 1
    wm8994_write(codec, 0x0302, 0x4000);    // AIF1 Master Slave Setting. To prevent that the music is played slowly.
    wm8994_write(codec, 0x0312, 0x0000);    // AIF2 Master Slave Setting
    wm8994_write(codec, 0x0310, 0x4118);    // AIF2 Control 1
	wm8994_write(codec, 0x0311, 0x0000);    // AIF2 Control 2
    wm8994_write(codec, 0x0520, 0x0080);    // AIF2 DAC Filter1
    wm8994_write(codec, 0x0204, 0x0019);    // AIF2 Clocking 1

    /* Input Path Routing */
    wm8994_write(codec, 0x0028, 0x0040);    // Input Mixer 2. SPK Mic using the IN2NL | DM1CDAT1
    wm8994_write(codec, 0x0002, 0x6280);    // SPK Mic using the IN2NL | DM1CDAT1

    if(!wm8994->testmode_config_flag)
	{
		wm8994_write(codec, 0x0029, 0x0100);    // Input Mixer 3. SPK Mic using the IN2NL | DM1CDAT1
    }
    wm8994_write(codec, 0x0004, 0x2002);	// Power Management 4
    wm8994_write(codec, 0x0604, 0x0010);    // DAC2 Left Mixer Routing
	wm8994_write(codec, 0x0605, 0x0010);    // DAC2 Right Mixer Routing
    audio_ctrl_mic_bias_gpio(1);

    /* Output Path Routing */
    wm8994_write(codec, 0x0005, 0x3303);    // Power Management 5
    wm8994_write(codec, 0x0003, 0x0300);    // Power Management 3
    wm8994_write(codec, 0x0601, 0x0005);    // DAC1 Left Mixer Routing. '0x0001' is added for a playback. (Original = 0x0004)
    wm8994_write(codec, 0x0602, 0x0001);    // DAC1 Right Mixer Routing(Playback)
#ifdef STEREO_SPEAKER_SUPPORT
    wm8994_write(codec, 0x0024, 0x0011);    // SPKOUT Mixers
#else
    wm8994_write(codec, 0x0024, 0x0010);    // SPKOUT Mixers
#endif
    wm8994_write(codec, 0x0420, 0x0080);    // AIF2 DAC Filter1(Playback)

    /* Input Path Volume */
    if(!wm8994->testmode_config_flag)
	{
        wm8994_write(codec, 0x0019, 0x0100 | TUNING_CALL_SPK_INPUTMIX_VOL);    // Left Line Input 3&4 Volume. SPK Mic using the IN2NL | DM1CDAT1
        wm8994_write(codec, 0x0603, 0x000C);    // DAC2 Mixer Volumes
        wm8994_write(codec, 0x0612, 0x01C0);    // DAC2 Left Volume
        wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume
        wm8994_write(codec, 0x0500, 0x01EF);    // AIF2 ADC Left Volume
    }

    /* Output Path Volume */
    if(!wm8994->testmode_config_flag)
	{
		wm8994_write(codec, 0x0022, TUNING_SPKMIXL_ATTEN);    // SPKMIXL Attenuation
#if 0
        wm8994_write(codec, 0x0026, (0x0140 | TUNING_CALL_SPKL_VOL));    // Speaker Volume Left
#else
        if(wm8994->codec_state & CALL_ACTIVE)
        {
            wm8994_write(codec, 0x0026, (0x0140 | TUNING_CALL_SPKL_VOL));    // Speaker Volume Left
        }
        else
        {
		    wm8994_write(codec, 0x0026, 0x0100);    // Speaker Volume Left
        }
#endif
#ifdef STEREO_SPEAKER_SUPPORT
        wm8994_write(codec, 0x0025, ((TUNING_CALL_CLASSD_VOL << 0x0003) | (TUNING_MP3_CLASSD_VOL << 0x0000)));    // SPKOUT Boost
#else
        wm8994_write(codec, 0x0025, (TUNING_CALL_CLASSD_VOL << 0x0003));    // SPKOUT Boost
#endif
		wm8994_write(codec, 0x0610, (0x0100 | TUNING_DAC1L_VOL));    // DAC1 Left Volume
	}

    wm8994_write(codec, 0x0006, 0x0000);    // Power Management 6. Prevent the mute when the audio transfer is executed from the bluetooth.
    wm8994_write(codec, 0x0621, 0x01C0);    // Sidetone
	wm8994_write(codec, 0x0036, 0x0003);

#ifdef STEREO_SPEAKER_SUPPORT
    wm8994_write(codec, 0x0001, 0x3003);    // Power Management 1
#else
	wm8994_write(codec, 0x0001, 0x1003);    // Power Management 1
#endif
//    msleep(50);
    wm8994_write(codec, 0x0224, 0x0C98);    // FLL1 Control(5). To set again the sampling rate for a AP sound.

#else
	DEBUG_LOG("");

	audio_ctrl_mic_bias_gpio(1);

	wm8994_write(codec, 0x0039, 0x006C);    // Anti Pop 2
    
	wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
	wm8994_write(codec, 0x0002, 0x6280);    // SPK Mic using the IN2NL | DM1CDAT1
	wm8994_write(codec, 0x0003, 0x0300);    // Power Management 3

	wm8994_write(codec, 0x0028, 0x0040);    // Input Mixer 2. SPK Mic using the IN2NL | DM1CDAT1
	
	wm8994_write(codec, 0x0004, 0x2002);	// Power Management 4
	wm8994_write(codec, 0x0005, 0x3303);    // Power Management 5
	wm8994_write(codec, 0x0006, 0x0000);    // Power Management 6

	if(!wm8994->testmode_config_flag)
	{
		wm8994_write(codec, 0x0029, 0x0180);    // Input Mixer 3. SPK Mic using the IN2NL | DM1CDAT1 + 30dB
        wm8994_write(codec, 0x0019, 0x0100 | 0x17);    // Left Line Input 3&4 Volume. SPK Mic using the IN2NL | DM1CDAT1 , spk mic gain up to 30dB -15dB

        /* SPKMIXL Attenuation */
		val = wm8994_read(codec, 0x0022);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, 0x0022, val);

        /* SPKMIXR Attenuation */
		val = wm8994_read(codec, 0x0023);
		val &= ~(WM8994_SPKMIXR_VOL_MASK); 
		val |= TUNING_SPKMIXR_ATTEN;	
		wm8994_write(codec, 0x0023, val);

        /* Speaker Volume Left */
#if 1
        wm8994_write(codec, 0x0026, (0x0140 | TUNING_CALL_SPKL_VOL));    // Speaker Volume Left
#else
        if(wm8994->codec_state & CALL_ACTIVE)
        {
            wm8994_write(codec, 0x0026, (0x0140 | TUNING_CALL_SPKL_VOL));    // Speaker Volume Left
        }
        else
        {
		    wm8994_write(codec, 0x0026, 0x0100);    // Speaker Volume Left
        }
#endif

        /* Speaker Volume Right */
		val = wm8994_read(codec,0x0027);
		val &= ~(0x0040 | 0x003F);
		wm8994_write(codec, 0x0027, val);

        /* SPKOUT Boost */
		val = wm8994_read(codec,0x0025);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= TUNING_CALL_CLASSD_VOL << 0x0003;
#ifdef STEREO_SPEAKER_SUPPORT
        val |= (TUNING_MP3_CLASSD_VOL << 0x0000);
#endif
		wm8994_write(codec, 0x0025, val);

        /* DAC1 Left Volume */
		val = wm8994_read(codec,0x0610);
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec, 0x0610, val);

        /* DAC1 Right Volume */
		val = wm8994_read(codec, 0x0611); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL;
		wm8994_write(codec, 0x0611, val);
	}

    /* SPKOUT Mixers */
	val = wm8994_read(codec, 0x0024);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= 0x0010;
#ifdef STEREO_SPEAKER_SUPPORT
    val |= 0x0001;    // SPKMIXR To SPKOUTR
#endif
	wm8994_write(codec, 0x0024, val);
	
	wm8994_write(codec, 0x0036, 0x0003);

    wm8994_write(codec, 0x0420, 0x0000);    // AIF2 DAC Filter1(Playback)
	wm8994_write(codec, 0x0520, 0x0000);    // AIF2 DAC Filter1
	wm8994_write(codec, 0x0601, 0x0005);    // DAC1 Left Mixer Routing
	wm8994_write(codec, 0x0602, 0x0005);    // DAC1 Right Mixer Routing
	wm8994_write(codec, 0x0603, 0x000C);    // DAC2 Mixer Volumes
	wm8994_write(codec, 0x0612, 0x01C0);    // DAC2 Left Volume
	wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume
	wm8994_write(codec, 0x0620, 0x0000);    // Oversampling
	wm8994_write(codec, 0x0621, 0x01C0);    // Sidetone

	wm8994_write(codec, 0x0604, 0x0010);    // DAC2 Left Mixer Routing
	wm8994_write(codec, 0x0605, 0x0010);    // DAC2 Right Mixer Routing

	wm8994_write(codec, 0x0244, 0x0C81);    // FLL2 Control 5
	wm8994_write(codec, 0x0241, 0x2F00);    // FLL2 Control 2
	wm8994_write(codec, 0x0243, 0x0600);    // FLL2 Control 4
	wm8994_write(codec, 0x0240, 0x0001);    // FLL2 Control 1

	wm8994_write(codec, 0x0208, 0x000F);    // Clocking 1. '0x000A' is added for a playback. (original = 0x0007)

	wm8994_write(codec, 0x0204, 0x0009);    // AIF2 Clocking 1
	wm8994_write(codec, 0x0211, 0x0003);    // AIF2 Rate
	wm8994_write(codec, 0x0310, 0x4118);    // AIF2 Control 1
	wm8994_write(codec, 0x0311, 0x4000);    // AIF2 Control 2 pcm format is changed ulaw to linear

	wm8994_write(codec, 0x0702, 0x8100);    // GPIO 3. Speech PCM Clock
	wm8994_write(codec, 0x0703, 0x8100);    // GPIO 4. Speech PCM Sync
	wm8994_write(codec, 0x0704, 0x8100);    // GPIO 5. Speech PCM Data Out
	wm8994_write(codec, 0x0706, 0x0100);    // GPIO 7. Speech PCM Data Input
#ifdef STEREO_SPEAKER_SUPPORT
    wm8994_write(codec, 0x0001, 0x3003);    // Power Management 1
#else
	wm8994_write(codec, 0x0001, 0x1003);    // Power Management 1
#endif  
#endif
}

void wm8994_set_voicecall_bluetooth(struct snd_soc_codec *codec)
{
#if 1
struct wm8994_priv *wm8994 = codec->private_data;

/* Audio Interface & Clock Setting */
    wm8994_write(codec, 0x0204, 0x0008);    // AIF2 Clocking 1. Clock Source Select
    wm8994_write(codec, 0x0208, 0x000F);    // Clocking 1. '0x000A' is added for a playback. (original = 0x0007)
    wm8994_write(codec, 0x0620, 0x0000);    // Oversampling
    wm8994_write(codec, 0x0211, 0x0003);    // AIF2 Rate
    wm8994_write(codec, 0x0302, 0x4000);    // AIF1 Master Slave Setting. To prevent that the music is played slowly.
    wm8994_write(codec, 0x0312, 0x0000);    // AIF2 Master Slave Setting
    wm8994_write(codec, 0x0310, 0x4118);    // AIF2 Control 1
    wm8994_write(codec, 0x0311, 0x0000);    // AIF2 Control 2 pcm format is changed ulaw to linear
    wm8994_write(codec, 0x0520, 0x0080);    // AIF2 DAC Filter 1
    wm8994_write(codec, 0x0204, 0x0009);    // AIF2 Clocking 1. AIF2 Clock Enable

    /* Input Path Routing */
    wm8994_write(codec, 0x0002, 0x4000);    // Power Management 2
    wm8994_write(codec, 0x0004, 0x3000);    // Power Management 4
    wm8994_write(codec, 0x0604, 0x0007);    // DAC2 Left Mixer Routing(Playback)
    wm8994_write(codec, 0x0605, 0x0007);    // DAC2 Right Mixer(Playback)
    
    wm8994_write(codec, 0x0039, 0x0068);    // Anti Pop2
    wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
    wm8994_write(codec, 0x0015, 0x0040);    
    wm8994_write(codec, 0x0704, 0x8100);    // GPIO 5. Speech PCM OUT
    wm8994_write(codec, 0x0706, 0x0100);    // GPIO 7. Speech PCM IN
    wm8994_write(codec, 0x0702, 0x8100);    // GPIO 3. Speech PCM CLK
    wm8994_write(codec, 0x0703, 0x8100);    // GPIO 4. Speech PCM SYNC
    wm8994_write(codec, 0x0707, 0x8100);    // GPIO 8. BT PCM DOUT
    wm8994_write(codec, 0x0708, 0x0100);    // GPIO 9. BT PCM DIN
    wm8994_write(codec, 0x0709, 0x0100);    // GPIO 10. BT PCM SYNC
    wm8994_write(codec, 0x070A, 0x0100);    // GPIO 11. BT PCM CLK
    wm8994_write(codec, 0x0244, 0x0C81);    // FLL2 Control 5
    wm8994_write(codec, 0x0241, 0x2F00);    // FLL2 Control 2
    wm8994_write(codec, 0x0242, 0x0000);    // FLL2 Control 3
    wm8994_write(codec, 0x0243, 0x0600);    // FLL2 Control 4
    wm8994_write(codec, 0x0240, 0x0001);    // FLL2 Cotnrol 1
    wm8994_write(codec, 0x004C, 0x1F25);    // Charge Pump 1
    wm8994_write(codec, 0x0006, 0x000C);    // Power Management 6. Input = GPIO8, Output = AIF2
    wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
    wm8994_write(codec, 0x0003, 0x0000);    // Power Management 3
    wm8994_write(codec, 0x0005, 0x3303);    // Power Management 5. '0x3300' is added for a playback. (Original = 0x0003)
    wm8994_write(codec, 0x0420, 0x0080);    // AIF1 DAC1 FIlter(Playback)
	wm8994_write(codec, 0x0402, (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL));    // AIF1 DAC1 Left Volume(Playback)
#if 0
    wm8994_write(codec, 0x0612, 0x01C0);    // DAC2 Left Volume
    wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume
#else
    if(wm8994->codec_state & CALL_ACTIVE)
    {
    	wm8994_write(codec, 0x0612, 0x01C0);    // DAC2 Left Volume
    	wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume
    }
    else
    {
        wm8994_write(codec, 0x0612, 0x0100);    // DAC2 Left Volume(Playback)
    	wm8994_write(codec, 0x0613, 0x0100);    // DAC2 Right Volume(Playback)
    }
#endif
    wm8994_write(codec, 0x0015, 0x0000);

#else
	wm8994_write(codec, 0x0620, 0x0000);    // Oversampling
    wm8994_write(codec, 0x0039, 0x0068);    // Anti Pop2
    wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
    wm8994_write(codec, 0x0015, 0x0040);    
    wm8994_write(codec, 0x0006, 0x0014);    // Power Management 6
    wm8994_write(codec, 0x0208, 0x0007);    // Clocking 1
    wm8994_write(codec, 0x0310, 0x0118);    // AIF2 Control 1
    wm8994_write(codec, 0x0311, 0x0000);    // AIF2 Control 2
    wm8994_write(codec, 0x0312, 0x0000);    // AIF2 Master Slave Setting
    wm8994_write(codec, 0x0704, 0x8100);    // GPIO 5. Speech PCM OUT
    wm8994_write(codec, 0x0706, 0x0100);    // GPIO 7. Speech PCM IN
    wm8994_write(codec, 0x0702, 0x8100);    // GPIO 3. Speech PCM CLK
    wm8994_write(codec, 0x0703, 0x8100);    // GPIO 4. Speech PCM SYNC
    wm8994_write(codec, 0x0707, 0x8100);    // GPIO 8. BT PCM DOUT
    wm8994_write(codec, 0x0708, 0x0100);    // GPIO 9. BT PCM DIN
    wm8994_write(codec, 0x0709, 0x0100);    // GPIO 10. BT PCM SYNC
    wm8994_write(codec, 0x070A, 0x0100);    // GPIO 11. BT PCM CLK
    wm8994_write(codec, 0x0211, 0x0003);    // AIF2 Rate
    wm8994_write(codec, 0x0244, 0x0C83);    // FLL2 Control 5
    wm8994_write(codec, 0x0241, 0x2F00);    // FLL2 Control 2
    wm8994_write(codec, 0x0242, 0x0000);    // FLL2 Control 3
    wm8994_write(codec, 0x0243, 0x0600);    // FLL2 Control 4
    wm8994_write(codec, 0x0240, 0x0001);    // FLL2 Cotnrol 1
    wm8994_write(codec, 0x0204, 0x0009);    // AIF2 Clocking 1. MCLK2
    wm8994_write(codec, 0x004C, 0x1F25);    // Charge Pump 1
    wm8994_write(codec, 0x0002, 0x4000);    // Power Management 2
    wm8994_write(codec, 0x0004, 0x0000);    // Power Management 4
    wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
    wm8994_write(codec, 0x0003, 0x0000);    // Power Management 3
    wm8994_write(codec, 0x0005, 0x0003);    // Power Management 5
    wm8994_write(codec, 0x0015, 0x0000);
#endif
}

void wm8994_set_voicecall_vps(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;
	int val;

    wm8994_write(codec, 0x0039, 0x0068);    // Anti Pop2
    wm8994_write(codec, 0x0001, 0x0003);    // Power Management 1
    wm8994_write(codec, 0x0015, 0x0040);
    wm8994_write(codec, 0x0702, 0x8100);    // GPIO 3. Speech PCM Clock
    wm8994_write(codec, 0x0703, 0x8100);    // GPIO 4. Speech PCM Sync
    wm8994_write(codec, 0x0704, 0x8100);    // GPIO 5. Speech PCM Data Out
    wm8994_write(codec, 0x0706, 0x8100);    // GPIO 7. Speech PCM Data Input
    wm8994_write(codec, 0x0244, 0x0C81);    // FLL2 Control 5
    wm8994_write(codec, 0x0241, 0x2F00);    // FLL2 Control 2
    wm8994_write(codec, 0x0243, 0x0600);    // FLL2 Control 4
    wm8994_write(codec, 0x0240, 0x0001);    // FLL2 Control 1
    msleep(3);

    /* Audio Interface & Clock Setting */
    wm8994_write(codec, 0x0204, 0x0008);    // AIF2 Clocking 1. Clock Source Select
    wm8994_write(codec, 0x0208, 0x000F);    // Clocking 1. '0x000A' is added for a playback. (original = 0x0007)
    wm8994_write(codec, 0x0620, 0x0000);    // Oversampling
    wm8994_write(codec, 0x0211, 0x0003);    // AIF2 Rate
    wm8994_write(codec, 0x0312, 0x0000);    // AIF2 Master Slave Setting
    wm8994_write(codec, 0x0310, 0x4118);    // AIF2 Control 1
    wm8994_write(codec, 0x0311, 0x4000);    // AIF2 Control 2
    wm8994_write(codec, 0x0520, 0x0000);    // AIF2 DAC Filter 1
    wm8994_write(codec, 0x0204, 0x0009);    // AIF2 Clocking 1. AIF2 Clock Enable

    /* Input Path Routing */
    wm8994_write(codec, 0x0028, 0x0030);    // Input Mixer 2
    wm8994_write(codec, 0x0002, 0x6240);    // Power Management 2
    wm8994_write(codec, 0x0029, 0x0030);    // Input Mixer 3
    wm8994_write(codec, 0x0004, 0x2002);    // Power Management 4
    wm8994_write(codec, 0x0604, 0x0010);    // DAC2 Left Mixer Routing
    audio_ctrl_mic_bias_gpio(1);
    
    /* Output Path Routing */
    wm8994_write(codec, 0x0005, 0x2303);    // Power Management 5. '0x0303' is added for a playback. (Original = 0x2002)
    wm8994_write(codec, 0x0601, 0x0005);    // DAC1 Left Mixer Routing. '0x0001' is added for a playback. (Original = 0x0004)
    wm8994_write(codec, 0x0602, 0x0001);    // DAC1 Right Mixer Routing(Playback)
    wm8994_write(codec, 0x002D, 0x0001);    // Output Mixer 1
    wm8994_write(codec, 0x002E, 0x0001);    // Output Mixer 2(Playback)
    wm8994_write(codec, 0x0003, 0x00F0);    // Power Management 3. '0x00F0' is added for a playback. (Original = 0x00A0)
    wm8994_write(codec, 0x0035, 0x0021);    // Line Mixer2
    wm8994_write(codec, 0x0420, 0x0000);    // AIF1 DAC1 FIlter(Playback)

    /* Input Path Volume */
    if(!wm8994->testmode_config_flag)
    {
        /* Left Line Input 1&2 Volume */
        val = wm8994_read(codec, 0x0018);
        val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
        val |= (0x0100 | TUNING_CALL_RCV_INPUTMIX_VOL);
        wm8994_write(codec, 0x0018, val);

        wm8994_write(codec, 0x0500, 0x01C0);    // AIF2 ADC Left Volume
        wm8994_write(codec, 0x0612, 0x01C0);    // DAC2 Left Volume
        wm8994_write(codec, 0x0603, 0x000C);    // DAC2 Mixer Volumes
    }
    wm8994_write(codec, 0x0621, 0x01C0);    // Sidetone

    /* Output Path Volume */
    if(!wm8994->testmode_config_flag)
    {
        /* Output Mixer 5 */
        val = wm8994_read(codec, 0x0031);
        val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
        val |= TUNING_RCV_OUTMIX5_VOL << 0x0009;
        wm8994_write(codec,0x0031, val);

        /* Output Mixer 6 */
        val = wm8994_read(codec, 0x0032);
        val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
        val |= TUNING_RCV_OUTMIX6_VOL << 0x0009;
        wm8994_write(codec,0x0032, val);

        /* DAC1 Left Volume */
        val = wm8994_read(codec,0x0610);
        val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
        val |= TUNING_DAC1L_VOL; 
        wm8994_write(codec, 0x0610, val);

        /* DAC1 Right Volume */
        val = wm8994_read(codec, 0x0611); 
        val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
        val |= TUNING_DAC1R_VOL;
        wm8994_write(codec, 0x0611, val);
        
        /* Left OPGA Volume */
        val = wm8994_read(codec, 0x0020);
        val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
        val |= (0x0100 | 0x0040 | TUNING_RCV_OPGAL_VOL);
        wm8994_write(codec,0x0020, val);

        /* Right OPGA Volume */
        val = wm8994_read(codec, 0x0021);
        val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
        val |= (0x0100 | 0x0040 | TUNING_RCV_OPGAR_VOL);
        wm8994_write(codec,0x0021, val);

        /* Line Output2 Volume */
        wm8994_write(codec, 0x001E, 0x0006);
    }
    wm8994_write(codec, 0x0015, 0x0000);    
    wm8994_write(codec, 0x0038, 0x0040);    // Anti Pop 1

    /* Sidetone */
    wm8994_write(codec, 0x0600, 0x0003);    // DAC1 Mixer Volume

    wm8994_write(codec, 0x0001, 0x0803);    // Power Management 1

    DEBUG_LOG("");
}


void wm8994_set_fmradio_common(struct snd_soc_codec *codec, int onoff)
{	
	struct wm8994_priv *wm8994 = codec->private_data;
	
	u16 val;

	DEBUG_LOG("onoff = [%d]", onoff);

	wm8994_write(codec, 0x39, 0x8);	//Cross Talk (H/W requested)

	if(onoff)
	{
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME); 	
		val &= ~(WM8994_IN2L_VU_MASK | WM8994_IN2L_MUTE_MASK | WM8994_IN2L_VOL_MASK);

		if(wm8994->fmradio_path == FMR_HP)
			val |= (WM8994_IN2L_VU | TUNING_FMRADIO_EAR_INPUTMIXL_VOL);
		else
			val |= (WM8994_IN2L_VU | TUNING_FMRADIO_SPK_INPUTMIXL_VOL);
		
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME); 	
		val &= ~(WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);

		if(wm8994->fmradio_path == FMR_HP)
			val |= (WM8994_IN2R_VU_MASK | TUNING_FMRADIO_EAR_INPUTMIXL_VOL);
		else
			val |= (WM8994_IN2R_VU_MASK | TUNING_FMRADIO_SPK_INPUTMIXL_VOL);
			
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);

		// Input mixer setting - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.02.25
	//	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
	//	val &= ~(WM8994_IN2LN_TO_IN2L_MASK | WM8994_IN2RN_TO_IN2R_MASK);
	//	val |= (WM8994_IN2LN_TO_IN2L | WM8994_IN2RN_TO_IN2R);
		val = (WM8994_IN2LN_TO_IN2L | WM8994_IN2RN_TO_IN2R);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val); 	

		if(!wm8994->testmode_config_flag)
		{
			// IN2L to MIXINL
			val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val &= ~(WM8994_IN2L_TO_MIXINL_MASK);
			val |= WM8994_IN2L_TO_MIXINL;
			wm8994_write(codec, WM8994_INPUT_MIXER_3, val);

			//IN2R to MIXINR
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val &= ~(WM8994_IN2R_TO_MIXINR_MASK);
			val |= WM8994_IN2R_TO_MIXINR;
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val);	
		}
		
		//DRC for Noise-gate (AIF2)
		wm8994_write(codec, WM8994_AIF2_ADC_FILTERS, 0xF800);
		wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0036);
		wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_2, 0x0010);
		wm8994_write(codec, WM8994_AIF2_DRC_2, 0x0840);
		wm8994_write(codec, WM8994_AIF2_DRC_3, 0x2400);
		wm8994_write(codec, WM8994_AIF2_DRC_4, 0x0000);
		wm8994_write(codec, WM8994_AIF2_DRC_5, 0x0000);
		wm8994_write(codec, WM8994_AIF2_DRC_1, 0x019C);
	}
	else
	{		
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME);	
		val &= ~(WM8994_IN2L_VU_MASK | WM8994_IN2L_MUTE_MASK | WM8994_IN2L_VOL_MASK);
		val |= (WM8994_IN2L_VU | WM8994_IN2L_MUTE);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);
		val &= ~(WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
		val |= (WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE);
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);

		val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
		val &= ~(WM8994_IN2LN_TO_IN2L | WM8994_IN2RN_TO_IN2R);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val);
		
		if(!wm8994->testmode_config_flag)
		{
			// IN2L to MIXINL
			val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val &= ~(WM8994_IN2L_TO_MIXINL_MASK);
			wm8994_write(codec, WM8994_INPUT_MIXER_3, val);

			//IN2R to MIXINR
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val &= ~(WM8994_IN2R_TO_MIXINR_MASK);
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val);	
		}
	}		
}

void wm8994_set_fmradio_headset(struct snd_soc_codec *codec)
{	
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;
	
	u16 nReadServo4Val = 0;
	u16 ncompensationResult = 0;
	u16 nCompensationResultLow=0;
	u16 nCompensationResultHigh=0;
	u8  nServo4Low = 0;
	u8  nServo4High = 0;
	
	DEBUG_LOG_INFO("Routing ear path : FM Radio -> EAR Out");

	wm8994->fmradio_path = FMR_HP;

	wm8994_disable_fmradio_path(codec, FMR_SPK);

	//DAC1 Setting
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);	//601H : 0x05
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF1DAC2L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_DAC1L | WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);	//602H : 0x05
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF1DAC2R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	//* Headset
	wm8994_write(codec, 0x102, 0x0003);
	wm8994_write(codec, 0x56, 0x0003);
	wm8994_write(codec, 0x102, 0x0000);

	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);

	val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

	val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	
	// Disable reg sync to MCLK
	val = wm8994_read(codec, WM8994_AIF1_CLOCKING_1); 	
	val &= ~(WM8994_AIF1CLK_ENA_MASK);
	val |= WM8994_AIF1CLK_ENA;
	wm8994_write(codec, WM8994_AIF1_CLOCKING_1, val);


	// Analogue Path Config
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2  ); 	
	val &= ~(WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
	val |= (WM8994_MIXINL_ENA | WM8994_MIXINR_ENA| WM8994_IN2L_ENA| WM8994_IN2R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2 , val );

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1); 	
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_NORMAL);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1 , 0x0003);
		
	//* Unmutes
	// Output setting
	if(!wm8994->testmode_config_flag)
	{
	val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME); 	
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_FMRADIO_OUTPUTL_VOL);
	wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

	val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME); 	
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_FMRADIO_OUTPUTR_VOL);
	wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_FMRADIO_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_FMRADIO_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}	

	wm8994_set_fmradio_common(codec, 1);
	
	//FLL2 Setting
	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);	//204H : 0x0011
	val &= ~(WM8994_AIF2CLK_ENA_MASK | WM8994_AIF2CLK_SRC_MASK);
	val |= (WM8994_AIF2CLK_ENA | 0x2 << WM8994_AIF2CLK_SRC_SHIFT);
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);	//208H : 0xF -> 0xE
	val &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK | WM8994_DSP_FS2CLK_ENA_MASK | WM8994_DSP_FS1CLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FS2CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);	//04H
	val &= ~(WM8994_AIF2ADCL_ENA_MASK | WM8994_AIF2ADCR_ENA_MASK | WM8994_ADCL_ENA_MASK | WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	//DAC2 Setting
	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C);	//603H : 0x018C

	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	val |= (WM8994_ADC1_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	val |= (WM8994_ADC1_TO_DAC2R);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_LEFT_VOLUME);	//612 : 1C0
	val &= ~(WM8994_DAC2L_MUTE_MASK | WM8994_DAC2L_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2L_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME,val);
	
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_VOLUME);	//613 : 1C0
	val &= ~(WM8994_DAC2R_MUTE_MASK | WM8994_DAC2R_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2R_VOL); //0 db volume	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0000);	//480 : 0
	 
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);	//03 : F
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);
	
//AIF2 Master/Slave , LOOPBACK, AIF2DAC Unmute
	val = wm8994_read(codec, WM8994_AIF2_MASTER_SLAVE);	//312 : 7000
	val &= ~(WM8994_AIF2_LRCLK_FRC_MASK | WM8994_AIF2_CLK_FRC_MASK | WM8994_AIF2_MSTR_MASK);
	val |= (WM8994_AIF2_LRCLK_FRC | WM8994_AIF2_CLK_FRC | WM8994_AIF2_MSTR);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, val);

	val = wm8994_read(codec, WM8994_AIF2_CONTROL_2);	//311 : 4001
	val &= ~(WM8994_AIF2_LOOPBACK_MASK);
	val |= (WM8994_AIF2_LOOPBACK);
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	wm8994_write(codec, WM8994_SIDETONE, 0x01c0);

	//* DC Servo Series Count
	val = 0x03E0;
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//* HP first and second stage
	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL |WM8994_HPOUT1R_ENA	 |WM8994_HPOUT1L_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = (WM8994_HPOUT1L_DLY | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Enable Charge Pump	
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~WM8994_CP_ENA_MASK ;
	val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, 0x9F25);

	msleep(5);

	//Digital  Mixer setting
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);	//2D : 1
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);	//2E : 1
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);	
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	//* DC Servo 
	val = (WM8994_DCS_TRIG_SERIES_1 | WM8994_DCS_TRIG_SERIES_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x0303 );

	msleep(160);

	nReadServo4Val=wm8994_read(codec,WM8994_DC_SERVO_4);
	nServo4Low=(signed char)(nReadServo4Val & 0xff);
	nServo4High=(signed char)((nReadServo4Val>>8) & 0xff);

	nCompensationResultLow=((signed short)nServo4Low -5)&0x00ff;
	nCompensationResultHigh=((signed short)(nServo4High -5)<<8)&0xff00;
	ncompensationResult=nCompensationResultLow|nCompensationResultHigh;
	wm8994_write(codec,WM8994_DC_SERVO_4, ncompensationResult);

	val = (WM8994_DCS_TRIG_DAC_WR_1 | WM8994_DCS_TRIG_DAC_WR_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, val );

	msleep(20);

	//* Headphone Output
		// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
	val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
		WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
	val |= (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
		WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//DAC1, DAC2 Volume Setting
	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );	//610H : 1C0
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);	//611 : 1C0
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);

	//DAC1 Unmute
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);

	val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);	//520 : 0
	val &= ~(WM8994_AIF2DAC_MUTE_MASK);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val);

}
	
void wm8994_set_fmradio_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG_INFO("Routing spk path : FM Radio -> SPK Out");

	wm8994_disable_fmradio_path(codec, FMR_HP);

	wm8994->fmradio_path = FMR_SPK;

	//Disable end point for preventing pop up noise.
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	wm8994_write(codec, WM8994_GPIO_3, 0x0100);	//702
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);	//703
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);	//704
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);	//705
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);	//706

	// Enable IN2 and MIXIN - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
//	val &= ~(WM8994_TSHUT_ENA_MASK | WM8994_TSHUT_OPDIS_MASK | WM8994_OPCLK_ENA_MASK | 
//			WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
//	val |= (WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_OPCLK_ENA | WM8994_MIXINL_ENA | 
//			WM8994_MIXINR_ENA | WM8994_IN2L_ENA | WM8994_IN2R_ENA);
	val = (WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_OPCLK_ENA | WM8994_MIXINL_ENA | 
			WM8994_MIXINR_ENA | WM8994_IN2L_ENA | WM8994_IN2R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	//AIF2 clock source <- FLL1
	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);	//204H : 0x0011
	val &= ~(WM8994_AIF2CLK_ENA_MASK | WM8994_AIF2CLK_SRC_MASK);
	val |= (WM8994_AIF2CLK_ENA | 0x2 << WM8994_AIF2CLK_SRC_SHIFT);
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);	//208H : 0xF -> 0xE
	val &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK | WM8994_DSP_FS2CLK_ENA_MASK | WM8994_DSP_FS1CLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FS2CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);	//04H
	val &= ~(WM8994_AIF2ADCL_ENA_MASK | WM8994_AIF2ADCR_ENA_MASK | WM8994_ADCL_ENA_MASK | WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	//DAC2 Setting
	//ADCR_TO_DAC2 vol, ADCL_TO_DAC2 vol
	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C); //603H : 0x018C
	
	//ADCL_TO_DAC2L
	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	val |= (WM8994_ADC1_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	//ADCR_TO_DAC2R
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	val |= (WM8994_ADC1_TO_DAC2R);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	//DAC Routing
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);	//601H : 0x05
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF1DAC2L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_DAC1L | WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);


	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);	//602H : 0x05
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF1DAC2R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	//DAC block volume
	//DAC1, DAC2 Volume Setting
	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );	//610H : 1C0
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);	//611 : 1C0
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);


	val = wm8994_read(codec, WM8994_DAC2_LEFT_VOLUME);	//612 : 1C0
	val &= ~(WM8994_DAC2L_MUTE_MASK | WM8994_DAC2L_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2L_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME,val);
	
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_VOLUME);	//613 : 1C0
	val &= ~(WM8994_DAC2R_MUTE_MASK | WM8994_DAC2R_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2R_VOL); //0 db volume	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0000);	//480 : 0


	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);


	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);	//03 : 0100
	val &= ~(WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
	val |= (WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

//	wm8994_write(codec, 0x01, 0x1003);
	
	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);

	//AIF2 Master/Slave , LOOPBACK, AIF2DAC Unmute
	val = wm8994_read(codec, WM8994_AIF2_MASTER_SLAVE); //312 : 7000
	val &= ~(WM8994_AIF2_LRCLK_FRC_MASK | WM8994_AIF2_CLK_FRC_MASK | WM8994_AIF2_MSTR_MASK);
	val |= (WM8994_AIF2_LRCLK_FRC | WM8994_AIF2_CLK_FRC | WM8994_AIF2_MSTR);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, val);

	val = wm8994_read(codec, WM8994_AIF2_CONTROL_2);	//311 : 4001
	val &= ~(WM8994_AIF2_LOOPBACK_MASK);
	val |= (WM8994_AIF2_LOOPBACK);
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);	//520 : 0
	val &= ~(WM8994_AIF2DAC_MUTE_MASK);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val);
	
	wm8994_set_fmradio_common(codec, 1);

	if(!wm8994->testmode_config_flag)
	{	
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
			
		val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
	
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_FMRADIO_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);
	
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);
	
		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= TUNING_FMRADIO_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);		
	}
	
	/*Output MIxer-Output PGA*/
	val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	// Output mixer setting
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_MIXINL_TO_SPKMIXL_MASK | WM8994_MIXINR_TO_SPKMIXR_MASK | WM8994_DAC1L_TO_SPKMIXL_MASK | WM8994_DAC1R_TO_SPKMIXR_MASK);
	val |= (WM8994_DAC1L_TO_SPKMIXL);
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	//Enbale bias,vmid and Left speaker
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_SPKOUTL_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	wm8994_write(codec, 0x420, 0x0000);

}

void wm8994_set_fmradio_headset_mix(struct snd_soc_codec *codec)
{	
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;

	DEBUG_LOG_INFO("");
	
	if(wm8994->fmradio_path == FMR_SPK)
		wm8994_set_playback_headset(codec);
	else
	{
		// Unmute the AF1DAC1	
		val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1 );	
		val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
		val |= WM8994_AIF1DAC1_UNMUTE;
		wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1 ,val);
		
		// Enable the Timeslot0 to DAC1L
		val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING  ); 	
		val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
		val |= WM8994_AIF1DAC1L_TO_DAC1L;
		wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING ,val);
				
		//Enable the Timeslot0 to DAC1R
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING  );	
		val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
		val |= WM8994_AIF1DAC1R_TO_DAC1R;
		wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING ,val);
		
		// Enable DAC1L to HPOUT1L path
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
		val &=	~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
		val |= WM8994_DAC1L_TO_MIXOUTL ;	
		wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
		
		// Enable DAC1R to HPOUT1R path
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
		val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
		val |= WM8994_DAC1R_TO_MIXOUTR;
		wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
				
		//Enable Dac1 and DAC2 and the Timeslot0 for AIF1	
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5 );	
		val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK |	WM8994_AIF1DAC1L_ENA_MASK );
		val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA  | WM8994_DAC1L_ENA |WM8994_DAC1R_ENA );
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_5 ,val);
				
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
		
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}
}

void wm8994_set_fmradio_speaker_mix(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;
	
	DEBUG_LOG_INFO("");

	if(wm8994->fmradio_path == FMR_HP)
		wm8994_set_playback_speaker(codec);
	else
	{
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);

		//Unmute the DAC path
		val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
		val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
		val |= WM8994_DAC1L_TO_SPKMIXL;
		wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

		// Eable DAC1 Left and timeslot left
		val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);	
		val &= ~( WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
		val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | WM8994_DAC1L_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);   

		//Unmute
		val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
		val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
		val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
		wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

		//enable timeslot0 to left dac
		val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
		val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
		val |= WM8994_AIF1DAC1L_TO_DAC1L;
		wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);
	}
	wm8994->fmradio_path = FMR_SPK_MIX;
}

void wm8994_set_fmradio_speaker_headset_mix(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;
	
	u16 nReadServo4Val = 0;
	u16 ncompensationResult = 0;
	u16 nServo4Low = 0;
	u16 nServo4High = 0;
	u8  nCompensationResultLow=0;
	u8 nCompensationResultHigh=0;

	DEBUG_LOG_INFO("");

	if(wm8994->fmradio_path == FMR_HP)
		wm8994_disable_fmradio_path(codec, FMR_HP);
	else
		wm8994_disable_fmradio_path(codec, FMR_SPK);
	
	wm8994->fmradio_path = FMR_DUAL_MIX;

	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);

	// Disable reg sync to MCLK
	val = wm8994_read(codec, WM8994_AIF1_CLOCKING_1); 	
	val &= ~(WM8994_AIF1CLK_ENA_MASK);
	val |= WM8994_AIF1CLK_ENA;
	wm8994_write(codec, WM8994_AIF1_CLOCKING_1, val);


	// Analogue Path Config
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2  ); 	
	val &= ~(WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
	val |= (WM8994_MIXINL_ENA | WM8994_MIXINR_ENA| WM8994_IN2L_ENA| WM8994_IN2R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2 , val );

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1); 	
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_NORMAL);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1 , 0x0003);
		
	val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

	val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

//	wm8994_set_fmradio_common(codec, 1);

	//FLL2 Setting
	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);	//204H : 0x0011
	val &= ~(WM8994_AIF2CLK_ENA_MASK | WM8994_AIF2CLK_SRC_MASK);
	val |= (WM8994_AIF2CLK_ENA | 0x2 << WM8994_AIF2CLK_SRC_SHIFT);
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);	//208H : 0xF -> 0xE
	val &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK | WM8994_DSP_FS2CLK_ENA_MASK | WM8994_DSP_FS1CLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FS2CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);	//04H
	val &= ~(WM8994_AIF2ADCL_ENA_MASK | WM8994_AIF2ADCR_ENA_MASK | WM8994_ADCL_ENA_MASK | WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	//DAC2 Setting
	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C);	//603H : 0x018C

	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	val |= (WM8994_ADC1_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	val |= (WM8994_ADC1_TO_DAC2R);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);
	
	//DRC for Noise-gate (AIF2)
	wm8994_write(codec, 0x541, 0x0850);
	wm8994_write(codec, 0x542, 0x0800);
	wm8994_write(codec, 0x543, 0x0001);
	wm8994_write(codec, 0x544, 0x0008);
	wm8994_write(codec, 0x540, 0x01BC);

	//DAC1 Setting
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);	//601H : 0x05
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF1DAC2L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_DAC1L | WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);


	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);	//602H : 0x05
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF1DAC2R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	//DAC1, DAC2 Volume Setting
	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );	//610H : 1C0
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);	//611 : 1C0
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);


	val = wm8994_read(codec, WM8994_DAC2_LEFT_VOLUME);	//612 : 1C0
	val &= ~(WM8994_DAC2L_MUTE_MASK | WM8994_DAC2L_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2L_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME,val);
	
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_VOLUME);	//613 : 1C0
	val &= ~(WM8994_DAC2R_MUTE_MASK | WM8994_DAC2R_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2R_VOL); //0 db volume	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0000);	//480 : 0
	 
	//Digital  Mixer setting
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);	//03 : F
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK
		|WM8994_SPKLVOL_ENA_MASK | WM8994_SPKRVOL_ENA_MASK);
	val |= (WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_SPKLVOL_ENA);	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);	//2D : 1
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);	//2E : 1
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);	
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_SPKOUT_MIXERS );
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	//Unmute the DAC path
	val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= WM8994_DAC1L_TO_SPKMIXL;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);	

	//AIF1 FLL Setting	
	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);
	
	//AIF2 Master/Slave , LOOPBACK, AIF2DAC Unmute
	val = wm8994_read(codec, WM8994_AIF2_MASTER_SLAVE);	//312 : 7000
	val &= ~(WM8994_AIF2_LRCLK_FRC_MASK | WM8994_AIF2_CLK_FRC_MASK | WM8994_AIF2_MSTR_MASK);
	val |= (WM8994_AIF2_LRCLK_FRC | WM8994_AIF2_CLK_FRC | WM8994_AIF2_MSTR);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, val);

	val = wm8994_read(codec, WM8994_AIF2_CONTROL_2);	//311 : 4001
	val &= ~(WM8994_AIF2_LOOPBACK_MASK);
	val |= (WM8994_AIF2_LOOPBACK);
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	//* Unmutes
	// Output setting
	if(!wm8994->testmode_config_flag)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_FMRADIO_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_FMRADIO_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_FMRADIO_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_FMRADIO_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= TUNING_FMRADIO_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);		

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_FMRADIO_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);		
	}	
	//* Headset
	wm8994_write(codec, 0x102, 0x0003);
	wm8994_write(codec, 0x56, 0x0003);
	wm8994_write(codec, 0x102, 0x0000);
	wm8994_write(codec, 0x5D, 0x0002);


	//* DC Servo Series Count
	val = 0x03E0;
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//* HP first and second stage
	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_HPOUT1L_ENA_MASK | WM8994_HPOUT1R_ENA_MASK | WM8994_SPKOUTR_ENA_MASK | WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_HPOUT1R_ENA | WM8994_HPOUT1L_ENA | WM8994_SPKOUTL_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = (WM8994_HPOUT1L_DLY | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);


	//Enable Charge Pump	
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~WM8994_CP_ENA_MASK ;
	val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, 0x9F25);

	msleep(5);

	//* DC Servo 
	val = (WM8994_DCS_TRIG_SERIES_1 | WM8994_DCS_TRIG_SERIES_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x0303 );

	msleep(160);

	nReadServo4Val=wm8994_read(codec,WM8994_DC_SERVO_4);
	nServo4Low=(u8)(nReadServo4Val & 0xff);
	nServo4High=(u8)((nReadServo4Val>>8) & 0xff);

	nCompensationResultLow=((u16)nServo4Low-5)&0x00ff;
	nCompensationResultHigh=(((u16)nServo4High-5)<<8)&0xff00;
	ncompensationResult=nCompensationResultLow|nCompensationResultHigh;
	wm8994_write(codec,WM8994_DC_SERVO_4, ncompensationResult);

	val = (WM8994_DCS_TRIG_DAC_WR_1 | WM8994_DCS_TRIG_DAC_WR_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, val );

	msleep(20);

	//* Headphone Output
		// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
	val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
		WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
	val |= (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
		WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//DAC1 Unmute
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);	
	
	val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);	//520 : 0
	val &= ~(WM8994_AIF2DAC_MUTE_MASK);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val);

}

#if defined WM8994_REGISTER_DUMP
void wm8994_register_dump(struct snd_soc_codec *codec)
{
	int wm8994_register;

	for(wm8994_register = 0; wm8994_register <= 0x6; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x15, wm8994_read(codec, 0x15));

	for(wm8994_register = 0x18; wm8994_register <= 0x3C; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x4C, wm8994_read(codec, 0x4C));

	for(wm8994_register = 0x51; wm8994_register <= 0x5C; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x60, wm8994_read(codec, 0x60));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x101, wm8994_read(codec, 0x101));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x110, wm8994_read(codec, 0x110));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x111, wm8994_read(codec, 0x111));

	for(wm8994_register = 0x200; wm8994_register <= 0x212; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x220; wm8994_register <= 0x224; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x240; wm8994_register <= 0x244; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x300; wm8994_register <= 0x317; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x400; wm8994_register <= 0x411; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x420; wm8994_register <= 0x423; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x440; wm8994_register <= 0x444; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x450; wm8994_register <= 0x454; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x480; wm8994_register <= 0x493; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x4A0; wm8994_register <= 0x4B3; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x500; wm8994_register <= 0x503; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x510, wm8994_read(codec, 0x510));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x520, wm8994_read(codec, 0x520));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x521, wm8994_read(codec, 0x521));

	for(wm8994_register = 0x540; wm8994_register <= 0x544; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x580; wm8994_register <= 0x593; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x600; wm8994_register <= 0x614; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x620, wm8994_read(codec, 0x620));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x621, wm8994_read(codec, 0x621));

	for(wm8994_register = 0x700; wm8994_register <= 0x70A; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
		
}
#endif
void wm8994_mute_voicecall_path(struct snd_soc_codec *codec, int path)
{
    if(path == RCV)
    {
        /* Output Path Volume */
        wm8994_write(codec, 0x0610, 0x0200);    // DAC1 Left Volume
        wm8994_write(codec, 0x0611, 0x0200);    // DAC1 Right Volume
        wm8994_write(codec, 0x0020, 0x0040);    // Left OPGA Volume
        wm8994_write(codec, 0x0021, 0x0040);    // Right OPGA Volume
        wm8994_write(codec, 0x001F, 0x0020);    // HPOUT2 Volume

        /* Output Path Routing */
        wm8994_write(codec, 0x0033, 0x0000);    // HPOUT2 Mixer. '0x0008' is added for a playback. (Original = 0x0010)
        wm8994_write(codec, 0x0420, 0x0200);    // AIF1 DAC1 FIlter(Playback)

        /* Input Path Volume */
        wm8994_write(codec, 0x0018, 0x008B);    // Left Line Input 1&2 Volume
        wm8994_write(codec, 0x0612, 0x0200);    // DAC2 Left Volume
        wm8994_write(codec, 0x0603, 0x0000);    // DAC2 Mixer Volumes
    
        DEBUG_LOG("===========================> The receiver voice path is muted.");
    }
    else if(path == HP)
    {
        /* Output Path Volume */
        if(tty_mode == TTY_MODE_HCO)
        {
            wm8994_write(codec, 0x0020, 0x0000);    // Left OPGA Volume
            wm8994_write(codec, 0x0021, 0x0000);    // Right OPGA Volume
            wm8994_write(codec, 0x001F, 0x0020);    // HPOUT2 Volume
            wm8994_write(codec, 0x0610, 0x0200);    // DAC1 Left Volume
            wm8994_write(codec, 0x0611, 0x0200);    // DAC1 Right Volume
            wm8994_write(codec, 0x0033, 0x0000);    // HPOUT2 Mixer
        }
        else
        {
            wm8994_write(codec, 0x001C, 0x0100);    // Left Output Volume
            wm8994_write(codec, 0x001D, 0x0100);    // Right Output Volume
            wm8994_write(codec, 0x0020, 0x0000);    // Left OPGA Volume
            wm8994_write(codec, 0x0021, 0x0000);    // Right OPGA Volume
            wm8994_write(codec, 0x0610, 0x0200);    // DAC1 Left Volume
            wm8994_write(codec, 0x0611, 0x0200);    // DAC1 Right Volume
            wm8994_write(codec, 0x0060, 0x0000);    // Analogue HP 1
        }

        /* Output Path Routing */
        if(tty_mode == TTY_MODE_HCO)
        {
            wm8994_write(codec, 0x0033, 0x0000);    // HPOUT2 Mixer
            wm8994_write(codec, 0x0420, 0x0200);    // AIF1 DAC1 Filter1
        }
        else
        {
            wm8994_write(codec, 0x0060, 0x0000);    // Analogue HP 1
            wm8994_write(codec, 0x0420, 0x0200);    // AIF1 DAC1 Filter1
        }

        /* Input Path Volume */
        if(tty_mode == TTY_MODE_VCO)
        {
            wm8994_write(codec, 0x0018, 0x008B);    // Left Line Input 1&2 Volume
        }
        else
        {
            wm8994_write(codec, 0x001A, 0x008B);    // Right Line Input 1&2 Volume
        }
        wm8994_write(codec, 0x0612, 0x0200);    // DAC2 Left Volume
        wm8994_write(codec, 0x0613, 0x0200);    // DAC2 Right Volume
        wm8994_write(codec, 0x0603, 0x0000);    // DAC2 Mixer Volumes
        DEBUG_LOG("===========================> The headset voice path is muted.");
    }
    else if(path == SPK)
    {
        /* Output Path Volume */
        wm8994_write(codec, 0x0025, 0x0000);    // SPKOUT Boost
		wm8994_write(codec, 0x0026, 0x0100);    // Speaker Volume Left
		wm8994_write(codec, 0x0027, 0x0000);    // Speaker Volume Right 
      	wm8994_write(codec, 0x0613, 0x0200);    // DAC2 Right Volume

        /* Output Path Routing */
        wm8994_write(codec, 0x0024, 0x0000);    // SPKOUT Mixers
        wm8994_write(codec, 0x0420, 0x0200);    // AIF2 DAC Filter1(Playback)

        /* Input Path Volume */
        wm8994_write(codec, 0x0019, 0x008B);    // Left Line Input 3&4 Volume. SPK Mic using the IN2NL | DM1CDAT1
        wm8994_write(codec, 0x0604, 0x0000);    // DAC2 Left Mixer Routing
    	wm8994_write(codec, 0x0605, 0x0000);    // DAC2 Right Mixer Routing

        DEBUG_LOG("===========================> The speaker voice path is muted.");
    }
    else if(path == BT)
    {
        /* Output Path Volume */
        wm8994_write(codec, 0x0420, 0x0200);    // AIF1 DAC1 FIlter(Playback)
        
        /* Input Path Routing */
        wm8994_write(codec, 0x0604, 0x0007);    // DAC2 Left Mixer Routing(Playback)
        wm8994_write(codec, 0x0605, 0x0007);    // DAC2 Right Mixer(Playback)
    
        wm8994_write(codec, 0x0707, 0x8000);    // GPIO 8. BT PCM DOUT
        wm8994_write(codec, 0x0708, 0x0000);    // GPIO 9. BT PCM DIN
        wm8994_write(codec, 0x0709, 0x0000);    // GPIO 10. BT PCM SYNC
        wm8994_write(codec, 0x070A, 0x0000);    // GPIO 11. BT PCM CLK

        /* Input Path Volume */
    	wm8994_write(codec, 0x0612, 0x0200);    // DAC2 Left Volume(Playback)
    	wm8994_write(codec, 0x0613, 0x0200);    // DAC2 Right Volume(Playback)
    	wm8994_write(codec, 0x0402, (WM8994_AIF1DAC1_VU | 0x0000));    // AIF1 DAC1 Left Volume(Playback)

        DEBUG_LOG("===========================> The bluetooth voice path is muted.");
    }
}

void wm8994_set_end_point_volume(struct snd_soc_codec *codec, int path)
{
    switch(path)
    {
        case RCV :
        {
            wm8994_write(codec, 0x001F, (TUNING_HPOUT2_VOL << 0x0004));    // HPOUT2 Volume

            DEBUG_LOG("===========================> The end point volume for a receiver is set.");
            break;
        }
        case HP :
        {
            if(tty_mode == TTY_MODE_HCO)
            {
                wm8994_write(codec, 0x001F, 0x0000);    // HPOUT2 Volume
            }
            else
            {
                wm8994_write(codec, 0x001C, (0x01C0 | TUNING_CALL_OUTPUTL_VOL));    // Left Output Volume
                wm8994_write(codec, 0x001D, (0x01C0 | TUNING_CALL_OUTPUTR_VOL));    // Right Output Volume
            }
            DEBUG_LOG("===========================> The end point volume for a headset is set.");
            break;
        }
        case SPK :
        {
            wm8994_write(codec, 0x0026, (0x01C0 | TUNING_CALL_SPKL_VOL));    // Speaker Volume Left

            DEBUG_LOG("===========================> The end point volume for a speaker is set.");
            break;
        }
        case BT :
        {
            wm8994_write(codec, 0x0612, 0x01C0);    // DAC2 Left Volume(Playback)
	        wm8994_write(codec, 0x0613, 0x01C0);    // DAC2 Right Volume(Playback)

            DEBUG_LOG("===========================> The end point volume for a bluetooth is set.");
            break;
        }
        default :
        {
            break;
        }
    }
}

