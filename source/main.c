#include <types.h>
#include <io.h>
#include <s5pv210/reg-timer.h>
#include <s5pv210/reg-gpio.h>
#include <lyrics.h>

#define TRUE 1
#define FALSE 0

u32_t loud = 2;

void on(void)
{
	writel(S5PV210_TCON, (readl(S5PV210_TCON) | (1<<12)));
}

void off(void)
{
	writel(S5PV210_TCON, (readl(S5PV210_TCON) & ~(1<<12)));
}

void delay(volatile u32_t t)
{
	t *= 1000000;
	while(t)
	{
		t--;
	}
}

static void do_music_initial(void)
{
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<8)) | (0x2<<8));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<2)) | (0x2<<2));

	writel(S5PV210_TCFG0, (readl(S5PV210_TCFG0) & ~(0xff<<8)) | (0x41<<8));
	writel(S5PV210_TCFG1, (readl(S5PV210_TCFG1) & ~(0xf<<8)) | (1<<8) );

	writel(S5PV210_TCON, (readl(S5PV210_TCON) & ~(0xf<<12)) | (1<<15) );
}

void set_fre_initial(u32_t fre)
{
	//TCNTB = 500KHz / fre;
	fre = 500000 / fre;
	writel(S5PV210_TCNTB2, fre);
	if(fre != zero)
	{
		writel(S5PV210_TCMPB2, loud * 10);
	}
	else
	{
		writel(S5PV210_TCMPB2, 0);
	}

	writel(S5PV210_TCON, (readl(S5PV210_TCON) | (1<<13)));
	writel(S5PV210_TCON, (readl(S5PV210_TCON) & ~(1<<13)));
}

enum {
	KEY_NAME_LEFT	= 0x1 << 1,
	KEY_NAME_DOWN	= 0x1 << 2,
	KEY_NAME_UP		= 0x1 << 3,
	KEY_NAME_RIGHT	= 0x1 << 4,
};

void do_key_initial(void)
{
	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0xf<<8)) | (0x0<<8));
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<4)) | (0x2<<4));

	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0xf<<12)) | (0x0<<12));
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<6)) | (0x2<<6));

	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<0)) | (0x0<<0));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<0)) | (0x2<<0));

	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<4))| (0x0<<4));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<2)) | (0x2<<2));
}

static u32_t __get_key_status(void)
{
	u32_t key = 0;

	if((readl(S5PV210_GPH0DAT) & (0x1<<2)) == 0)
		key |= KEY_NAME_LEFT;

	if((readl(S5PV210_GPH0DAT) & (0x1<<3)) == 0)
		key |= KEY_NAME_DOWN;

	if((readl(S5PV210_GPH2DAT) & (0x1<<0)) == 0)
		key |= KEY_NAME_UP;

	if((readl(S5PV210_GPH2DAT) & (0x1<<1)) == 0)
		key |= KEY_NAME_RIGHT;

	return key;
}

bool_t get_key_status(u32_t * key)
{
	static u32_t a = 0, b = 0, c = 0;

	a = __get_key_status();
	b = __get_key_status();
	c = __get_key_status();

	if((a == b) && (a == c))
	{
		*key = a;
		return TRUE;
	}

	return FALSE;
}

bool_t get_key_event(u32_t * keyup, u32_t * keydown)
{
	static u32_t key_old = 0x0;
	u32_t key;

	if(!get_key_status(&key))
		return FALSE;

	if(key != key_old)
	{
		*keyup = (key ^ key_old) & key_old;
		*keydown = (key ^ key_old) & key;
		key_old = key;

		return TRUE;
	}

	return FALSE;
}

u32_t * change_music(u32_t sequence)
{
	if(sequence == 0)
	{
		return Twinkle;
	}
	else if(sequence == 1)
	{
		return Happy_birthday;
	}
	else if(sequence == 2)
	{
		return Graze_The_Roof;
	}
	else
	{
		return The_March_of_the_Volunteers;
	}
}

void return_to_zero(u32_t * i,u32_t sequence)
{
	if(sequence == 0)
	{
		if(*i >= sizeof(Twinkle)/sizeof(Twinkle[0]))
		{
			*i = 0;
			delay(500);
		}
	}
	else if(sequence == 1)
	{
		if(*i >= sizeof(Happy_birthday)/sizeof(Happy_birthday[0]))
		{
			*i = 0;
			delay(500);
		}
	}
	else if(sequence == 2)
	{
		if(*i >= sizeof(Graze_The_Roof)/sizeof(Graze_The_Roof[0]))
		{
			*i = 0;
			delay(500);
		}
	}
	else
	{
		if(*i >= sizeof(The_March_of_the_Volunteers)/sizeof(The_March_of_the_Volunteers[0]))
		{
			*i = 0;
			delay(500);
		}
	}

}

void do_led_initial(void)
{
	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<12)) | (0x1<<12));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<6)) | (0x2<<6));

	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<16)) | (0x1<<16));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<8)) | (0x2<<8));

	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<20)) | (0x1<<20));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<10)) | (0x2<<10));

	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<4)) | (0x1<<4));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<2)) | (0x2<<2));
}

void close_led_all(void)
{
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x1<<3));
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x1<<4));
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<5)) | (0x1<<5));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<1)) | (0x1<<1));
}


int main(int argc, char * argv[])
{
	u32_t keyup, keydown;
	u32_t i = 0,sequence = 2;
	u32_t *music = change_music(sequence);
	bool_t flag = TRUE;

	do_key_initial();
	do_led_initial();
	do_music_initial();

	while(1)
	{
		if(get_key_event(&keyup, &keydown))
		{
			if(keydown != FALSE)
			{
				if(keydown & KEY_NAME_LEFT)
				{
					if(sequence != 0)
					{
						sequence--;
						i = 0;
						flag = FALSE;
						music = change_music(sequence);
						delay(200);
					}
				}
				if(keydown & KEY_NAME_RIGHT)
				{
					if(sequence != 3)
					{
						sequence++;
						i = 0;
						flag = FALSE;
						music = change_music(sequence);
						delay(200);
					}
				}
				if(keydown & KEY_NAME_UP)
				{
					if(loud != 6)
					{
						loud++;
					}
				}
				if(keydown & KEY_NAME_DOWN)
				{
					if(loud != 2)
					{
						loud--;
					}
				}
			}
		}

		close_led_all();
		switch(sequence)
		{
			case 0:
				writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x0<<3));
				break;
			case 1:
				writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x0<<4));
				break;
			case 2:
				writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<5)) | (0x0<<5));
				break;
			case 3:
				writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<1)) | (0x0<<1));
				break;
		}

		if(flag == TRUE)
		{
			return_to_zero(&i,sequence);
		}

		set_fre_initial(music[i]);
		on();
		flag = TRUE;

		delay(music[i+1]);

		off();
		delay(3);

		i += 2;
	}
	return 0;
}
