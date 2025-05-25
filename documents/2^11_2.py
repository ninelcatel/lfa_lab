max_exp=int(input("please enter a max exponent\n Beware that computing time might take a while if using large numbers \n"))

if max_exp > 100000000:
    print("Max exponent cannot be greater than 100.000.000 (using ~30MB ram)")
    exit()

def check_digitsEven(n):
    while n>0:
        if((n%10)%2!=0):
            return False
        n //= 10
    return True

print("-----Outputting all powers of 2 numbers smaller than 2^{max_exp} that have only even digits -----")
for exponent in range(1,max_exp+1):
    number = 2 ** exponent
    if(check_digitsEven(number)):
        print(f'{number} has only Even digits and is a power of 2 to the {exponent}')
    