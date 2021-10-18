#!/usr/bin/awk -f
BEGIN {
	FS = "[&=]"
	output = ARGV[2]
	line = ARGV[3]
	hide = ARGV[4]
	file = ARGV[5]

	cmd = "urlencode \"$(</dev/stdin)\""

	if (hide) 
		cmd = "stty -echo echonl; " cmd

	ARGV[2] = ARGV[3] = ARGV[4] = ARGV[5] = ""
}

NR == line {
	# Separate into url and data
	sep = " "
	if (split($0, url, sep) != 2) {
		sep = "?"
		if (split($0, url, sep) != 2) {
			print
			next
		}
	} 

	num_params = split(url[2], params)

	# Print parameter names
	for (i = 1; i <= num_params; i += 2)
		printf "%s %d ", params[i], i
	print ""

	# Set values
	while (getline input < "-") {
		p_nums_len = split(input, p_nums, ",")
		for (i = 1; i <= p_nums_len; ++i) {
			j = p_nums[i]
			if (j < 0) {
				params[-j + 1] = "@"
				continue
			}
			def = (hide) ? "" : params[j + 1]
			printf "%s [%s]: ", params[j], def
			cmd | getline params[j + 1]
			close(cmd)
		}
	}

	if (file) {
		"openssl rand -hex 8" | getline rnd
		prefix = "------------------------"

		for (i = 1; i <= num_params; i += 2) {
			print "--" prefix rnd > "f"
			print "Content-Disposition: form-data; name=\"" params[i] "\"" > "f"
			if (index(params[i + 1], "@") == 1) {
				print "Content-Type: application/octet-stream" > "f"
				params[i + 1] = ""
			}
			print "\n" params[i + 1] > "f"
			if (i >= num_params - 1)
				print "--" prefix rnd "--" > "f"
		}
	}

	# Print url
	printf url[1] sep > output
	for (i = 1; i < num_params; ++i)
		printf "%s%s", params[i], (i % 2) ? "=" : "&" > output
	print params[num_params] > output
}

NR != line {
	print > output
}
