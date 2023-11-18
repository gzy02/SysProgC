
int CountString(char *str)
{
    // Exercise:
    // Add your code here:
    int i = 0, status = 1, ans = 0;
    while (str[i])
    {
        if (str[i] == ' ')
        {
            status = 1;
        }
        else
        {
            if (status == 1)
                ++ans;
            status = 2;
        }
        ++i;
    }
    return ans;
}
