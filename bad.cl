class C {
	a : Int<-1+4;
	b : Bool<-8;
	c : Bool<- b;
	init(x : Int, y : Bool) : C {
           {
		a <- y;
		b <- y;
		self;
           }
	};
};
class W{
	cut (x:Int, y: Bool):Int{
		{1;}
	};


};
Class Main {
	main():C {4+3/4-3-2};
	main1(c:Int, x: Bool):C {
	 {
	  (new W).cut(1,2);
	  (new W)@W.cut(1,2);
	  (new C).init(1,true,3);
	  (new C).iinit(1,true);
	  (new C).init(1,true);
	  (new C)@C.init2(1,1);
	  (new C)@C.init(1,1,3);
	  (new C)@C.init(1,2);
	  (new C)@C.init(1,true);
	  (new C);
	  (new Int);
	  let w: Int <- 99 in 99+3*w;
	  4+3*6+x;
	 }
	};
};

Class P inherits Q {};
Class Q inherits Z {};
Class Z inherits P {};