# CSD
Combinatorial Subset Difference representation and its broadcast encryption

Under Construction:
Will be organized asap.

# Interval BE [Lin10]

Usage: ./interval option

option:

setup - setup msk_out_filename

	ex: ./interval setup sample_msk.key

keygen - keygen id msk_in_filename dk_out_filename

	ex: ./interval keygen 3 sample_msk.key sample_dk.key

encrypt - encrypt msg_filename interval_left interval_right hdr_out_filename

	ex: ./interval encrypt sample_msg.in 1 5 sample_hdr.dat

decrypt - decrypt hdr_in_filename dk_in_filename interval_left interval_right id msg_out_filename

	ex: ./interval decrypt sample_hdr.dat sample_dk.key 1 5 3 sample_msg.out

broadcast - broadcast msg_filename set_filename hdrlist_out_filename

	ex: ./interval broadcast sample_msg.in sample_sets.in sample_hdrlist.dat

receive - receive hdrlist_in dk_in_filename id msg_out_filename

	ex: ./interval receive sample_hdrlist.dat sample_dk.key 3 sample_msg.out

