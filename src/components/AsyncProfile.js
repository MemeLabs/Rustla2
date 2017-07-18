import Loadable from './Loadable';


const AsyncProfile = Loadable({
  loader: () => import(/* webpackChunkName: "profile" */ './Profile'),
});

export default AsyncProfile;
