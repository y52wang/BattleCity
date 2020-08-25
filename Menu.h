﻿/**************************************************
Autor: Damian "RippeR" Dyńdo
URL: http://warsztat.gd/projects.php?x=view&id=2063
**************************************************/

#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <string>
#include <vector>
#include "Sprites.h"

using namespace std;

class CMenu {
public:
    CMenu()
        : m_selected_item(1)
        , m_EnableLog(false)
    { }

    void Init();
    void ProcessEvents();
    void DrawMenu();
    void SelectItem(int id)  { m_selected_item = id; }
    void ChooseItem();

    int SelectedItem()  { return m_selected_item; }

    bool EnableLog()    { return m_EnableLog; }

private:
    int         m_selected_item;  //Aktualnie zaznaczona opcja
    SpriteData  m_Background;
    SpriteData  m_MenuOptions;
    SpriteData  m_Pointer;
    SpriteData  m_Logo;

    bool        m_EnableLog;    // 数据记录 开启/关闭
};

#endif // MENU_H_INCLUDED
