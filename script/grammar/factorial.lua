
function  fact_n(n)
	if n <= 1 then
		return 1;
	else
		return n*fact_n(n-1);
	end;
end

print("input a number:");
a = io.read("*number");
print(fact_n(a));