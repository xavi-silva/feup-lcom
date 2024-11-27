PROG = proj

.PATH: ${.CURDIR}/devices/
.PATH: ${.CURDIR}/objects/
.PATH: ${.CURDIR}/menu/

SRCS = main.c menu.c event.c button.c character.c game_logic.c interrupt_handler.c sprite.c game.c serial_port.c queue.c rtc.c graphics.c mouse.c KBC.c keyboard.c timer.c utils.c

CPPFLAGS += -pedantic
DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
