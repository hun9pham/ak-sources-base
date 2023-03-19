CFLAGS += -I./sources/driver/ssd1306
CPPFLAGS += -I./sources/driver/ssd1306
CFLAGS += -I./sources/driver/ssd1306/Soft_I2C
CPPFLAGS += -I./sources/driver/ssd1306/Soft_I2C

VPATH += sources/driver/ssd1306
VPATH += sources/driver/ssd1306/Soft_I2C

C_SOURCES += sources/driver/ssd1306/ssd1306.c
C_SOURCES += sources/driver/ssd1306/fonts.c
C_SOURCES += sources/driver/ssd1306/Soft_I2C/soft_i2c.c
