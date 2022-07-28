1. Select the GPIO of your choice
2. Check the GPIO PIN number from the P8 or P9 (EX. P8->11)
3. Get the PIN NAME from the Schematic Reference Manual (EX. GPIO1_13)
4. Get the GPIO file number by (1*32)+13 = 45 => GPIO_45
5. Use the GPIO_45 to set the directional
6. Set the Value
7. use cat ./direction command to check the value of the stored variable
8. To set a value use the command echo 1 > value
sys/class/gpio











THanks
https://youtu.be/pJWcRPcqk3g
