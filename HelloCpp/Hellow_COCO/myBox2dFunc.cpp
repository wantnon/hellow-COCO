//
//  myBox2dFunc.cpp
//  mybox2dtest
//
//  Created by apple on 13-7-23.
//
//

#include "myBox2dFunc.h"
b2Vec2 getAveragePoint(const vector<b2Vec2>&b2PointList){//��þ�ֵ��
	int npoint=b2PointList.size();
	assert(npoint!=0);
	b2Vec2 c(0,0);//һ��Ҫ��ʼ��������������ֵ
	for(int i=0;i<npoint;i++){
		const b2Vec2&v=b2PointList[i];
		c+=v;
	}
	c*=1.0/npoint;//�õ�wc
	return c;
}
b2AABB getAABB(b2Shape*shape,const b2Transform&transform){
	b2AABB aabb;
	int32 childIndex = 0;
	shape->ComputeAABB(&aabb, transform, childIndex);  
	return aabb;
}
int getFixtureCount(b2Body*body){
	int count=0;
	for (b2Fixture* f =body->GetFixtureList(); f; f = f->GetNext())
	{
		count++;
	}
	return count;
}
b2Fixture* getFixtureAt_backWards(b2Body*body,int index_backWards){//���fixtureList�е�����index��fixture
	//��fixture����
	//b2Body����m_fixtureCount��Ա��������û���ṩ���ʽӿ�,�������ǻ����Լ����
	const int fixtureCount=getFixtureCount(body);
	const int index_forwards=fixtureCount-1-index_backWards;
	int i=0;
	for (b2Fixture* f =body->GetFixtureList(); f; f = f->GetNext())
	{
		if(i==index_forwards)return f;
		i++;
	}
	return NULL;

}
b2BodyType bodyTypeStrTobodyTypeValue(string bodyTypeStr){
	if(bodyTypeStr=="staticBody"){
		return b2_staticBody;
	}else if(bodyTypeStr=="kinematicBody"){
		return b2_kinematicBody;
	}else if(bodyTypeStr=="dynamicBody"){
		return b2_dynamicBody;
	}else{
		cout<<"error��δ֪bodyTypeStr:"<<bodyTypeStr<<endl;
		system("pause");
		exit(0);
	}

}
void splitPolygonShapeWithHoriLine(b2PolygonShape&shape,b2Transform transform,float H_horiLine,
                                   bool&upperShapeGot,bool&lowerShapeGot,
                                   b2PolygonShape&upperShape,b2PolygonShape&lowerShape){//ʹ��ˮƽֱ���и�shape
    

    //�ж��Ƿ�Ϊpolygon shape
    if(shape.GetType()!=b2Shape::e_polygon){
        cout<<"error:shape���Ͳ���e_polygon!"<<endl;
        exit(0);
    }
    //��shape��aabb
    b2AABB aabb;
    {
        int32 childIndex = 0;
        shape.ComputeAABB(&aabb, transform, childIndex);
  //      cout<<"aabb range:"<<aabb.lowerBound.x<<" "<<aabb.upperBound.x<<endl;
    }
    //ˮƽ����shape�Ƿ��ཻ
    bool intersect=true;
    //���󽻵�
    b2Vec2 hitPoint_left;
    bool hit_left;
    const float margin=1.0;
    {
        b2Vec2 p1(aabb.lowerBound.x-margin ,H_horiLine);
        b2Vec2 p2(aabb.upperBound.x+margin,H_horiLine);
        b2RayCastInput input;
        input.p1=p1;
        input.p2=p2;
        input.maxFraction =1;;
        int32 childIndex = 0;
        b2RayCastOutput output;
        hit_left = shape.RayCast(&output, input, transform, childIndex);
        if (hit_left){
            hitPoint_left = input.p1 + output.fraction *(input.p2-input.p1);
        }
    }
    if(hit_left==false)intersect=false;
    //���ҽ���
    b2Vec2 hitPoint_right;
    bool hit_right;
    if(intersect){
        b2Vec2 p1(aabb.upperBound.x+margin,H_horiLine);
        b2Vec2 p2(aabb.lowerBound.x-margin,H_horiLine);
        b2RayCastInput input;
        input.p1=p1;
        input.p2=p2;
        input.maxFraction = 1;
        int32 childIndex = 0;
        b2RayCastOutput output;
        hit_right = shape.RayCast(&output, input, transform, childIndex);
        if (hit_right){
            hitPoint_right = input.p1 + output.fraction *(input.p2-input.p1);
        }
        if(hit_right==false)intersect=false;
    }//�õ�intersect
 //   cout<<"hit_left,hit_right:"<<hit_left<<" "<<hit_right<<endl;
    if(intersect==false){//������ཻ
        //�ж�shape��ˮƽ��֮�ϻ���֮��
        b2Vec2 shapeCentroid_world=b2Mul(transform, shape.m_centroid);//ע��Ҫת������ռ�
        if(shapeCentroid_world.y>H_horiLine){//��֮��
            upperShape=shape;
            upperShapeGot=true;
            lowerShapeGot=false;
        }else{//��֮��
            lowerShape=shape;
            lowerShapeGot=true;
            upperShapeGot=false;
        }
    }else{//����ཻ
        //��shape���ѳ�����������
        vector<b2Vec2> vlist_upper1;
        vector<b2Vec2> vlist_upper2;//����
        vector<b2Vec2> vlist_lower1;
        vector<b2Vec2> vlist_lower2;//����
        int nv=shape.GetVertexCount();
        bool isUpperVertex=false;//�Ƿ�Ϊupper�ڵ�
        for(int i=0;i<nv;i++){
            b2Vec2 v=shape.GetVertex(i);
            v=b2Mul(transform, v);//ת��������
            bool isUpperVertex_f=isUpperVertex;
            isUpperVertex=v.y>H_horiLine;
            if(i!=0&&isUpperVertex_f!=isUpperVertex){//���isUpperVertex�����仯�������i!=0���жϣ���Ϊ�׸������Ǿ������ܷ����仯�ģ�
                if(isUpperVertex==true){//����ǽ����Ϸ�
                    //���vlist_upper1Ϊ�գ���v���뵽vlist_upper1������Ӧ���뵽vlist_upper2
                    if(vlist_upper1.empty()){//��v���뵽vlist_upper1
                        vlist_upper1.push_back(v);
                    }else{//��v���뵽vlist_upper2
                        vlist_upper2.push_back(v);
                    }
                }else{//����ǽ����·�
                    //���vlist_lower1Ϊ�գ���v���뵽vlist_lower1������Ӧ���뵽vlist_lower2
                    if(vlist_lower1.empty()){//��v���뵽vlist_lower1
                        vlist_lower1.push_back(v);
                    }else{//��v���뵽vlist_lower2
                        vlist_lower2.push_back(v);
                    }
                }
            }else{//���isUpperVertexû�����仯
                if(isUpperVertex){
                    //���vlist_upper2Ϊ�գ�����뵽vlist_upper1�����vlist_upper2��Ϊ�գ�����뵽vlist_upper2
                    if(vlist_upper2.empty()){
                        vlist_upper1.push_back(v);
                    }else{
                        vlist_upper2.push_back(v);
                    }
                }else{
                    //���vlist_lower2Ϊ�գ�����뵽vlist_lower1�����vlist_lower2��Ϊ�գ�����뵽vlist_lower2
                    if(vlist_lower2.empty()){
                        vlist_lower1.push_back(v);
                    }else{
                        vlist_lower2.push_back(v);
                    }
                }
            }
        }//�õ�vlist_upper1,vlist_upper2,vlist_lower1,vlist_lower2
        //vlist_upper=[hitPoint_right,vlist_upper2,vlist_upper1,hitPoint_left]
    //    cout<<(int)vlist_upper1.size()<<" "<<(int)vlist_upper2.size()<<endl;
        vector<b2Vec2> vlist_upper;
        {
            vlist_upper.push_back(hitPoint_right);
            int nv2=(int)vlist_upper2.size();
            for(int i=0;i<nv2;i++){
                vlist_upper.push_back(vlist_upper2[i]);
            }
            int nv1=(int)vlist_upper1.size();
            for(int i=0;i<nv1;i++){
                vlist_upper.push_back(vlist_upper1[i]);
            }
            vlist_upper.push_back(hitPoint_left);
  
        }//�õ�vlist_upper
        //vlist_lower=[hitPoint_left,vlist_lower2,vlist_lower1,hitPoint_right]
        vector<b2Vec2> vlist_lower;
        {
            vlist_lower.push_back(hitPoint_left);
            int nv2=(int)vlist_lower2.size();
            for(int i=0;i<nv2;i++){
                vlist_lower.push_back(vlist_lower2[i]);
            }
            int nv1=(int)vlist_lower1.size();
            for(int i=0;i<nv1;i++){
                vlist_lower.push_back(vlist_lower1[i]);
            }
            vlist_lower.push_back(hitPoint_right);
        }//�õ�vlist_lower
   //     cout<<"vlist_upper:"<<(int)vlist_upper.size()<<endl;
   //     cout<<"vlist_lower:"<<(int)vlist_lower.size()<<endl;
        //��vlist_upper��vlist_lower�е������ת��Ϊshape�ֲ�����
        {
            int nv_upper=(int)vlist_upper.size();
            for(int i=0;i<nv_upper;i++){
                vlist_upper[i]=b2MulT(transform, vlist_upper[i]);
            }
            int nv_lower=(int)vlist_lower.size();
            for(int i=0;i<nv_lower;i++){
                vlist_lower[i]=b2MulT(transform, vlist_lower[i]);
            }
        }
        //�Ƴ����ڵ��غϵ�
        {
            const float eps=0.001;
            int nv_upper=(int)vlist_upper.size();
            for(int i=0;i<nv_upper;i++){
                b2Vec2&v=vlist_upper[i];
                b2Vec2&vf=vlist_upper[(i-1+nv_upper)%nv_upper];
                if(fabsf(v.x-vf.x)<eps&&fabsf(v.y-vf.y)<eps){//v��vf���
                    //ɾ��v
                    vlist_upper.erase(vlist_upper.begin()+i);
                    i--;//����i���ڴ�λ��
                    nv_upper--;//��Ҫ�������
                }
            }
            int nv_lower=(int)vlist_lower.size();
            for(int i=0;i<nv_lower;i++){
                b2Vec2&v=vlist_lower[i];
                b2Vec2&vf=vlist_lower[(i-1+nv_lower)%nv_lower];
                if(fabsf(v.x-vf.x)<eps&&fabsf(v.y-vf.y)<eps){//v��vf���
                    //ɾ��v
                    vlist_lower.erase(vlist_lower.begin()+i);
                    i--;//����i���ڴ�λ��
                    nv_lower--;//��Ҫ�������
                }
            }
        }

        //�������ؽ��
        if((int)vlist_upper.size()>=3&&(int)vlist_upper.size()<=b2_maxPolygonVertices){
            upperShapeGot=true;
            upperShape.Set(&vlist_upper[0], (int)vlist_upper.size());
        }else{
            upperShapeGot=false;
        }
        if((int)vlist_lower.size()>=3&&(int)vlist_lower.size()<=b2_maxPolygonVertices){
            lowerShapeGot=true;
            lowerShape.Set(&vlist_lower[0], (int)vlist_lower.size());
        }else{
            lowerShapeGot=false;
        }

  

    }
    
}
