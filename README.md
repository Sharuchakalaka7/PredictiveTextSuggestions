# Predictive Text Suggestions #

This program was initially made for the CS 240E course at the University of Waterloo. Since the initial commit consisting of the perfect program submission for the assignment here, many changes are to be made, including:
	* support a larger character set Σ' (currently the character set, Σ = {a-z}, only supports lowercase English letters)
	* user-friendly usage (instead of custom command line prompts)
	* inclusion of the Unix library for more realistic usage
	* incorporate into other text-based programs?...

The program runs quite efficiently in terms of runtime, able to easily withstand test sizes of order 10^8-10^9 primary function calls - asymptotic runtime is O(size of query string). Will need continue optimizing auxiliary space further, especially when expanding the alphabet size. Currently implemented as a modified compressed, flagged trie structure.

If you have any ideas or suggestions, please let me know through email, <ahmed.shahriar343@gmail.com>.
