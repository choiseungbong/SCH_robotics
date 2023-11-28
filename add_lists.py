def add_lists(foo, bar):
	minlen = min(len(foo), len(bar))
	dst = []
	for i in range(minlen):
		dst.append(foo[i] + bar[i])
	return dst

if __name__ == "__main__":
	dst = add_lists([1,2,3], [4,5,6])
	print(dst)
