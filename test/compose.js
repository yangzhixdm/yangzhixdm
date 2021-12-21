function compose (funs) {
  if (funs.length === 0) {
    return arg => arg
  }
  if (funs.length === 1) {
    return funs[0]
  }
  return funs.reduce((a, b) => {
    return function (...args) {
      return a(b(...args))
    }
  })
}


let fn = compose([function (data) {
  console.log(data)
  console.log(1)
  return {
    name: 1
  }
}])

fn({param: 1}, { param: 2})



