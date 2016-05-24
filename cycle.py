def getLabel(e):
	return str(int(round(e.length))) + " m"

def lengthLabel(e):
	return str(int(round(e.length))) + " m"

def cycleLabel(e):
	str_list = []
	for set in e.tagSets:
		if set.hasTag('ref'):
			str_list.append(set['ref'])
	
	str_list.sort()
	return '+'.join(str_list)
