#pragma once

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define SAFE_DELETE(a) delete (a); (a) = nullptr; 
#define CYCLIC_ERASE(a, b) assert((b >= 0) && (b < (a.size() - 1))); a[b] = a.back(); a.pop_back();
