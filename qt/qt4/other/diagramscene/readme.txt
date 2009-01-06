1. 不知道怎麼設定的,使得呼叫父類別的mousePressEvent()就可以點選物件,而呼叫父類別的mouseMoveEvent()就可以移動物件
答:在設定QGraphicsItem設定以下兩個參數
setFlag(QGraphicsItem::ItemIsMovable, true);        // 設定使得物件可以移動,找了一個下午總算找到了為什麼此物件可以被選擇了
setFlag(QGraphicsItem::ItemIsSelectable, true);     // 設定使得物件可以被選擇

2. 不知道為什麼當選取的物件可以按del直接刪除
答:在設定deleteAction時deleteAction->setShortcut(tr("Delete"));
設定的快捷鍵是Delete,並且把它加到itemMenu中,使得當此item被選取時,可以使用此快捷鍵
