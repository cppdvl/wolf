namespace DGE{
        template <typename T>
    class Singleton{
    inline static T* mt{nullptr};
    public:
        inline static T& GetInstanceReference()
        {
            if (!mt){
                mt = new T();
            }
            //static T t = T{};
            return *mt;
        }
    };

}