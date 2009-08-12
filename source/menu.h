#ifndef _MENU_H_
#define _MENU_H_

/* Prototypes */
void Menu_Device(void);
void Menu_Format(void);
void Menu_Install(void);
void Menu_Remove(void);
void Menu_Boot(void);
void Menu_Config(void);
void Menu_Update(void);
void Menu_CoverFetch(void);
void Menu_Loop(void);
void __Menu_PrintInfo(struct discHdr *header);

#endif
