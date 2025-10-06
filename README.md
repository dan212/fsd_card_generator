# fsd_card_generator
Tool to generate card from text input for Full Spectrum Dominance wargame

Cards are generated from text, each card separated by `"___"` on a new line. 
Text input supports using HTML tags for rich text.

## Card syntax
Each section of card must be a new line.
Each card must have at least 2 lines: Name and Class.
And entry that dosen't have them is not valid.
```
<Card name> - <X> pts 
<Classification>
```
Classification is written in to card text verbatum. Name and point value must be separated with "-"

If card is a unit, stat line is expected as well
```
Cmd <X>; Def <Y>+; Save d<Z>(<N>); <M>DU
```
Supported separators are `',', ':'` and `';'`

Systems are described as follows
```
-S<X>: [Y] Example system (token): Example descr
```
Where X is system index, and Y is AD description. If system has a token, it can be added with "(token)" keyword in system name.
Multiple AD slots can be added, each described as `[X]` for single value slot or `[X-Y]` as AD range. AD slots don't have seprators between them.

Any added tags must be written in a single line, starting with "Tags:" keyword. All text past `':'` is used as tags verbatum

Damage grid description starts with keyword "Damage:", followed by pairs of damage zones and values, separated by `';'`
Single damage zone example:``` [1] Dead```
Extended damage zone example: ```[3-4] S2```

Any other text that has no apporpriate keywords in the front is assumed as miscilenious and put in separate zone on the card verbatum.

### Meta-data:
Default card count can be entered with following line:
```m_cc: X```
Where X is amount of cards to generate by default

## Card import
After card text is pasted to Input zone, **"Add"** button will begin parcing and all generated cards will be listed in "Cards in system" list. Using spinbox in **"Count"** column, amount of cards to generate in sheets can be adjusted.
Alternatevly, new card template can be generated with "Generate template" button. This will create new card in the list. Selected card can be removed with **"Remove selected"** button.
After selecting card, it's text will be displayed in **"Current card text"** zone, and it's image will be shown in left side of the window. Card text can be edited online, and changes made will be visible on card render.
By default, generated cards have white background. Image background can be set with **"Set background image"** button, which will apply selected image to all cards. Image will be automatically scaled to card size by streching.

## Card sheet generation.
Pressing **"Generate cards"** will generate card sheets with each card having **"Count"** amount of copies. All card data is recorded in to generated sheets. Images will be saved in start up folder.
To load text data from sheets, use "Load from PNG sheets" button and select all pages to load. This will clean any previous data in application and generate card list again. Note that background card image is not saved in the sheets, and need to be selected again between application runs.
