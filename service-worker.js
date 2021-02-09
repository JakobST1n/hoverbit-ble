var APP_PREFIX = 'hoverbitcontroller'     // Identifier for this app (this needs to be consistent across every cache update)
var VERSION = '1.0.4-0'              // Version of the off-line cache (change this value everytime you want to update cache)
var CACHE_NAME = APP_PREFIX + VERSION
var URLS = ['fa-brands-400.0a2f2d3f.woff2','fa-brands-400.529a9178.svg','fa-brands-400.61a02662.eot','fa-brands-400.a43d027d.ttf','fa-brands-400.abaa6551.woff','fa-regular-400.12e902ff.woff2','fa-regular-400.4fc407aa.svg','fa-regular-400.779979e6.woff','fa-regular-400.b26d94f4.eot','fa-regular-400.d59f1e49.ttf','fa-solid-900.26506ae2.svg','fa-solid-900.50887a69.woff','fa-solid-900.6c3fc37b.eot','fa-solid-900.be8a4f74.woff2','fa-solid-900.f6ec8168.ttf','index.html','index.html.bak','main.3f46a61e.js','main.3f46a61e.js.map','manifest.webmanifest','maskable_icon_x128.ae1e74f9.png','maskable_icon_x144.fc14e37c.png','maskable_icon_x152.ce6c1441.png','maskable_icon_x384.1af8edb2.png','maskable_icon_x512.a9ac38fa.png','maskable_icon_x72.b43d35ec.png','maskable_icon_x96.e03bc2e8.png','pwa-192x192.b3dbd8bb.png','pwa-512x512.b27071b7.png','service-worker.js','service-worker.js.bak','styles.96ce15e4.css','styles.96ce15e4.css.map']  // This will be replaced by the deploy-script

// Respond with cached resources
self.addEventListener('fetch', function (e) {
  console.log('fetch request : ' + e.request.url)
  e.respondWith(
    caches.match(e.request).then(function (request) {
      if (request) { // if cache is available, respond with cache
        console.log('responding with cache : ' + e.request.url)
        return request
      } else {       // if there are no cache, try fetching request
        console.log('file is not cached, fetching : ' + e.request.url)
        return fetch(e.request)
      }

      // You can omit if/else for console.log & put one line below like this too.
      // return request || fetch(e.request)
    })
  )
})

// Cache resources
self.addEventListener('install', function (e) {
  e.waitUntil(
    caches.open(CACHE_NAME).then(function (cache) {
      console.log('installing cache : ' + CACHE_NAME)
      return cache.addAll(URLS)
    })
  )
})

// Delete outdated caches
self.addEventListener('activate', function (e) {
  e.waitUntil(
    caches.keys().then(function (keyList) {
      // `keyList` contains all cache names under your username.github.io
      // filter out ones that has this app prefix to create white list
      var cacheWhitelist = keyList.filter(function (key) {
        return key.indexOf(APP_PREFIX)
      })
      // add current cache name to white list
      cacheWhitelist.push(CACHE_NAME)

      return Promise.all(keyList.map(function (key, i) {
        if (cacheWhitelist.indexOf(key) === -1) {
          console.log('deleting cache : ' + keyList[i] )
          return caches.delete(keyList[i])
        }
      }))
    })
  )
})
