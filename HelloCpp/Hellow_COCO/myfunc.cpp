//
//  myfunc.cpp
//  mybox2dtest
//
//  Created by apple on 13-8-1.
//
//

#include "myfunc.h"
string toString(const char* cstr){
    if(cstr==NULL)return string();//���cstrΪNULL���򷵻ؿ�string
    string str=cstr;
    return str;
}
vector<float> strToValueList(const string&_str)
//��string����ȡ��ֵ
//���string�д������֡���š����Ż�ո�֮����ַ����򷵻��б�Ϊempty
//���_strΪ�ջ�ֻ�пո��򷵻��б�Ϊempty
{
    const vector<float> emptyValueList;
    if(_str.empty())return emptyValueList;
    //��str���油��һ���ո��Ա����һ����ֵ����ȡ����
    string str=_str+" ";
    vector<float> valueList;//��ֵ�б�
    string tstr;//������ȡ������ֵ���ִ�
    int len=(int)str.size();
    for(int i=0;i<len;i++){
        bool isNumberOrDotOrSpace=((str[i]>='0'&&str[i]<='9')||str[i]=='.'||str[i]=='-'||str[i]==' '||str[i]=='('||str[i]==')'||str[i]==',');//�����֣���ţ����ţ��ո����ţ�����
        if(isNumberOrDotOrSpace==false){
            cout<<"warning:�������֣���ţ����ţ��ո����ţ�����!"<<endl;
            return emptyValueList;//�������������ֵ�ַ������򷵻ؿ���ֵ�б�
        }
        if(str[i]!=' '&&str[i]!=','&&str[i]!=')'&&str[i]!='('){//������ǿո񣬶��ţ�����
            //���뵽tstr
            tstr+=str[i];
        }else{//����ǿո񣬶��ţ�����
            if(tstr.empty()){//���tstrΪ��
                //�޶���
            }else{//���tstr�ǿ�
                //tstr��ȡ���
                //��tstrת������ֵ����ӵ�valueList
                float value=atof(tstr.c_str());
                valueList.push_back(value);
                //��tstr��գ��Ա���ȡ��һ����ֵ���ִ�
                tstr.clear();
            }
        }
    }//�õ�valueList
    return valueList;

}

