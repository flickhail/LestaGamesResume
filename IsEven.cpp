// #1
bool isEven(int value)
{
  return value % 2 == 0;
}

// #2 
bool isEven(int value)
{
  return !(value & 0x0001);
}

// #1 is easy to read and looks much cleaner
// #2 is as fast as the second (it takes only 2 operations to evaluate) and can be even faster if we change the semantics to "isOdd()".
// Nevertheless, #2 is not so obvious and it can be hard to read
