#TODO

- [ ] Tokenize the input filename in order to get the actual file to prepend text to instead of the full path:  
Bad Example: one_examples/test.txt  
Good: examples/one_test.txt  
'/' and null '\0' are illegal chars in linux filenames. Tokenize the entire path by '/' and save the last token, this is the filename.

