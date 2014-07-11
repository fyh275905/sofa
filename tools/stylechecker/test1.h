#ifndef MYCLASS_H__
#define MYCLASS_H__

template<class T>
class Data
{
private:
    T* t;

} ;

template<class T>
class SingleLink
{
private:
    T* m_t;

} ;

template<class T>
class DualLink
{
private:
    T* m_t;

} ;

template<class T>
class OtherType
{
private:
    T* m_t;

} ;



namespace validnamespaceWithoutUsing {
	namespace InvalidNamespace{
	
	}
}


using namespace std ; // Interdit
using namespace validnamespaceWithoutUsing ; 

class MyClass
{
	public:
		MyClass(){}

		int functionInvalid(){ return 0; }

		int FunctionINVALID() ;
		int functionVALID() ; 

        int publicposition ;

        Data<int> 			d_DatafieldVALID ;
        SingleLink<int> 		d_SinglelinkVALID ;

        DualLink<int> 			d_DuallinkVALID ;
        OtherType<int> 			m_OthertypeVALID ;

	void* 				m_voidpointerVALID ;
	int* 				m_intpointerVALID ;

        int 				privatepositionINVALID ;
        int 				m_privatepositionVALID ;
        Data<int> 			private_datafieldINVALID ;
        SingleLink<int> 		private_singlelinkINVALID ;
        DualLink<int> 			private_duallinkINVALID ;
        OtherType<int> 			private_othertypeINVALID ;

	void* 				private_voidpointerINVALID;
	int* 				private_intpointerINVALID;

	bool 				invalidBooleanINVALID;
	bool 				m_isAValidBooleanVALID;
};

class myInvalidClass
{
};

class MyInvalid_Class
{
};



#endif // MCLASS_H__
