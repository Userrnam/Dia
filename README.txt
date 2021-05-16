control-z - undo
control-shift-z - redo
hold alt to disable snapping
+ - make grid twice bigger
- - make grid twice smaller

use middle button to move around

CreateMode (Left Button)
l - create lines
c - create circles
t - create text

control-c - exit from text mode to line

EditMode (Right Button)
hold shift to change circle radius
press t to modify selected text

Commands:

:set [scope].<target>.<option> <value>

scope:
	global - for all entities of type <target>
	default - change default value of type <target> for <option>
	<no option> - for selected entities of type <target>

target:
	line, circle, text, ui

option <line>: width, color
option <circle>: fill.color, border.color, border.width
option <text>: color, size, font
option <ui>: size, font

example:
	set line.color 0 0 255  - create selected elements of type <Line> blue
	set circle.border.width 4
	set ui.font SomeFont (SomeFont must be in the resources folder)

:load <project>
:save <project>
:exprot <file_name.png>

file 'defaults' contains default values for entites in the same format as for :set command

