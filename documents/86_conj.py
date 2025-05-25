def check_digits_for_zero(n):
    while n>0:
        if((n%10)==0):
            return True
        n //= 10
    return False

max_exp=int(input("please enter a max exponent\nBeware that computing time might take a while if using large numbers \n"))

if max_exp > 100000000:
    print("Max exponent cannot be greater than 100.000.000 (using ~30MB ram)")
    exit()

print("-----Outputting all powers of 2 numbers smaller than 2^{max_exp} that have at LEAST one digit equal to zero -----")
for exponent in range(1,max_exp+1):
    number = 2 ** exponent
    if(check_digits_for_zero(number)):
        print(f'{number} has at least one zero in its structure and its a power of 2 to {exponent}')
    else:
        print(f'{number} doesnt have a single zero in its structure and its a power of 2 to {exponent}')
    