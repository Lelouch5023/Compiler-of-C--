逻辑：1）Array > Basic > Structure
	  2）为区分Array，关键在于	
					VarDec -> ID  <1>
							 |VarDec2 LB INT RB  <2>
					对于<1>: ID.type := VarDec
					对于<2>: VarDec2.type := toArray(type = VarDec.type, size = INT.data)
					所以，VarDec必须有继承属性type

承上，为了让VarDec有继承属性type，必须找到Specifier把Specifier.type传给VarDec，即终极目的：VarDec.type := Specifier.type
走到VarDec的有3条路：
	A)	VarDec <- (ExtDecList <= Specifier)[ExtDef] 
	B)	(VarDec <= Specifier)[ParamDec]
	C)	VarDec <- Dec <- (DecList <= Specifier)[Def]

注意：1.Specifier要先遍历
	  2.注意是复制赋值还是地址赋值
	


对于 Specifier -> StructSpecifier | TYPE 而言，后者易处理，前者处理为：
	当	StructSpecifier -> STRUCT OptTag LC DefList RC	<1>
						 | STRUCT Tag	<2>
	StructSpecifier的类型仍然为object <1><2>，
	但是如果StructSpecifier后面有fieldlist <1>, 就把OptTag对应的ID存表（如果有的话， 没有就unique_name()）

	StructSpecifier.type.kind = OBJECT
	
最后关于DefList对于fieldlist的转换处理：
	







函数的处理：
	1. ret 
	2. param 注意：实际上是从VarList开始用的structure类型来连接的
	3. funcname







对于结构的修正：
DefList = [Def, Def, Def, Def, ...]
Def = Specifier DecList SEMI
DecList = [Dec, Dec, Dec, Dec, ...]
Dec = ...VarDec...

这时算法是：把Dec作原子，把DecList/Def作链表，然后在DefList把很多链表拼接起来
注意Dec.type最开始是Specifier传进来的继承属性
后来dfvisit了Dec以后，Dec.type应该是其ID的type，作综合属性
