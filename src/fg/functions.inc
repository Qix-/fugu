namespace fg {
	// step
	// pulse
	// clamp
 	// etc

	template <typename T> T min(T a, T b){
		return a<b?a:b; 
	}
	
	template <typename T> T lerp(T a, T b, T t){
		return (b-a)*t + a;
	}
}
