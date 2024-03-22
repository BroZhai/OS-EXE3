牌的大小排序:
2<3<4<5<6<7<8<9<T(10)<J<Q<K<A

游戏目标:
目标是尽量避免获得 红心牌 和 黑桃Queen（Queen of Spades）。
玩家在每一局中获得的红心和黑桃女士的数量越少，得分越低；最终得分最低的玩家获胜。
（提示，可以把这里的分理解成"排名"）

规则1:
一张玩家作为"先手"，"任意"打出一张牌，然后剩余的所有玩家都到打出和"先手"一样花色suit的牌
(如果一个玩家手中恰好没有所打的花色牌，那么他被允许出任何一张牌，但是打不同花色的牌的玩家肯定不会是那一轮的"赢家")
如果本轮中出现了黑桃Queen，那么出黑桃女士的玩家获得本轮的牌。
如果本轮中没有黑桃Queen，那么出最高点数的红心牌的玩家获得本轮的牌。

注:
1.如果玩家手中没有与首轮出的花色相同的牌，他们可以选择出一张黑桃牌或红心牌，但不可出黑桃女士。
2.如果玩家手中只有红心和黑桃女士，他们可以出任何一张牌(实在没退路了XD)

规则2：
每轮当中"牌值"最高的玩家会取得回合的四张牌，被得分最高的玩家收下后，那个玩家就会成为"先手"

在每一轮结束后，根据玩家在本轮中获得的红心牌和黑桃女士的数量计算得分。
每张红心牌计1分，黑桃女士计13分。
玩家的得分累加，直到有人达到或超过设定的得分上限。
获得最低总得分的玩家被宣布为胜利者。（考虑有人分到0的情况[牌全空了]）

为了简化我们的程序，我们不会像现实一样考虑那么多"策略":




SA>HA>CA>DA先分组 (花色)



首先思路和上一个assignmet很类似

首先把读txt文件的I/O问题解决了
然后再像上一个assignment那样"分牌"[OK]

第一轮的第一个玩家应该是child1 (player1)
玩家每次打出的牌即为 "最低的牌" (先看牌值，后看花色)

子玩家打出来的牌后 会被传回到 父进程中，然后再由父进程传给下一个玩家[上一个玩家打出的牌的信息]
下一个玩家需要打出和上一个玩家"花色"一样的牌
(牌值不重要，提前sort好每个suit的牌，当别人打出一个suit时，下一个玩家首先看自己对应的suit里面有没有牌，如果有，直接打sort好的第一张牌(lowest)[不在意牌值的大小])

当每轮每个玩家打完之后，父进程会判断谁的"牌值"最大，即为本场的"赢家"（寄）
本场的赢家作为下一轮的"先手"

(咱不考虑过多discard的情况)
注意，可能要用到waitpid()等待每个玩家"打完"

判定牌局最终的结束，有两种情况:
1.有玩家的分值到达0【没牌了】
2.有玩家"反其道而行之"，收集到了13张红心牌和一张黑桃Q(那么算他最终胜利，其他人被惩罚+26分)