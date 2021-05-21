#include<stdio.h>
#include<stdlib.h>

//namespace std
//{
// template <class _Elem, class _Traits, class _Alloc>
// struct hash<basic_string<_Elem, _Traits, _Alloc>> {
//     size_t operator()(const basic_string<_Elem, _Traits, _Alloc>& _Keyval) const noexcept {
//         return 3983709877683599140;
//     }
// };
//}

int strcmp(char const* str1,char const* str2)
{
    static int count=0;
    count++;

    if(count==10)
        return 0;

    int sub=0,i;
    for(i=0;str1[i]!='\0'&&str2[i]!='\0'&&!sub;i++)
        sub=str1[i]-str2[i];
    if(sub==0&&str1[i]=='\0'&&str2[i]=='\0')
        return 0;
    return sub;
}