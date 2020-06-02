# Effect-Build
A simple commandline frontend for D3DCompile to build FX 2.0 HLSL effect files, designed for FNA-based games

## Usage

`efb.exe <parameters> input.fx <output.fxo>`

### Parameters

`/I "<path>"`
  Add additional path to search for include files
  
`/Od`
  Disable optimization
  
`/O0, /O1, /O2, /O3`
  Optimization levels (0, 1, 2, 3). Default is /O1

`/Vd`
  Disable validation
  
`/WX`
  Treat warnings as errors

`/Zpc`
  Pack matrices in column-major order

`/Zpr`
  Pack matrices in row-major order
