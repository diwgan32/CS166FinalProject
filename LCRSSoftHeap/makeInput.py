import random

f = open("randomInput.txt", "w")
nums = []
for i in range(2000):
	nums.append(i)

random.shuffle(nums)

for num in nums:
	f.write("i " + str(num) + '\n')

for num in nums:
	f.write("d\n")
