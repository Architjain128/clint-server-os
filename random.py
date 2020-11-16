import random
import string
import math
n = int(input())
def randStr(chars = string.ascii_uppercase + string.digits, N=10):
	return ''.join(random.choice(chars) for _ in range(N))

str3 = randStr(chars = string.ascii_uppercase + string.ascii_lowercase + '.,?&; 0123456789\t\n', N=n)
print(str3)

